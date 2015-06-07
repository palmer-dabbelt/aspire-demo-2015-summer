/* Copyright 2015 Palmer Dabbelt <palmer@dabbelt.com> */

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <string.h>

#include "generate-calibration.h++"

#if defined(READ_FROM_INTERNAL_MEMORY) || defined(WRITE_TO_INTERNAL_MEMORY)
#include "generate-flat-test.h++"
#endif

#include "util.h++"

#ifndef CALIBRATE_MAX
#define CALIBRATE_MAX 0x8200
#define CALIBRATE_MIN 0x7e00
#endif

#define SIZE (208 * 156)

#ifdef __riscv_hwacha4
/* The actual Hwacha compute function. */
extern "C" void __hwacha_flat(void);

/*
 * va0: cold + i
 * va1: hot  + i
 * va2: raw  + i
 * va3: flat + i
 * vs0: 0.0f
 * vs1: 1.0f
 * vs2: 255.0f
 */
__asm__ volatile (
".align 3\n"
"__hwacha_flat:\n"
/* vv0:raw[i] */
/* vv2:cold[i] */
/* vv3:offset = raw[i] - cold[i] */
"  vlhu vv0, va0\n"
"  vlhu vv2, va2\n"
"  vsub vv3, vv2, vv0\n"

/* vv4:scale = hot[i] - cold[i] */
"  vlhu vv1, va1\n"
"  vsub vv4, vv1, vv0\n"

/* vv3:foffset = offset */
"  vfcvt.s.l vv3, vv3\n"

/* vv4:fscale = scale */
"  vfcvt.s.l vv4, vv4\n"

/* vv5:scaled = foffset / fscale */
"  vfdiv.s vv5, vv3, vv4\n"

/* if (scaled < 0.0) vv5:scaled = 1.0 */
"  vfmax.s vv5, vv5, vs0\n"

/* if (scaled > 1.0) vv5:scaled = 1.0 */
"  vfmin.s vv5, vv5, vs1\n"

/* flat[i] = ((1 << 8) - 1) * scaled */
"  vfmul.s vv5, vv5, vs2\n"
"  vfcvt.w.s vv5, vv5\n"
"  vsw vv5, va3\n"
"  vstop\n"
    );
#endif

int main(int argc __attribute__((unused)), 
         const char **argv __attribute__((unused)))
{
    auto cold = generate_calibration::cold;
    auto hot  = generate_calibration::hot;

#ifdef HAVE_FFPLAY
    auto ffplay = popen("ffplay -i - -f rawvideo -video_size 208x156 -pixel_format gray16le -loglevel quiet", "w");
    setbuf(ffplay, NULL);
#endif

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    uint16_t raw_storage[SIZE + 64];
    auto raw = aligned_alloc<uint16_t>(8, SIZE, &raw_storage[0]);
    uint16_t flat_storage[SIZE + 64];
    auto flat = aligned_alloc<uint16_t>(8, SIZE, &flat_storage[0]);

#ifdef __riscv_hwacha4
    /* We need a single configuration here  */
    size_t vector_length;
    __asm__ volatile (
        "vsetcfg 8, 1\n"
        "vsetvl %0, %1\n"
        : "=r"(vector_length)
        : "r"(SIZE)
        );
#endif

    /* Make sure all of memory has been paged in, since we're going to
     * be loading from the text section. */
    for (size_t i = 0; i < SIZE; ++i)
        flat[i] = raw[i] = hot[i] + cold[i];

    while (true) {
#ifdef READ_FROM_INTERNAL_MEMORY
        memcpy(&raw[0], generate_flat_test::input, SIZE * sizeof(*raw));
#else
        freadall(&raw[0],
                 SIZE * sizeof(*raw),
                 stdin);
#endif

#ifdef __riscv_hwacha4
        for (size_t i = 0; i <= SIZE; i += vector_length) {
            if (SIZE == i)
                continue;

            union {
                float f;
                uint32_t i;
            } zero;
            zero.f = 0.0f;

            union {
                float f;
                uint32_t i;
            } one;
            one.f = 1.0f;

            union {
                float f;
                uint32_t i;
            } twofivefive;
            twofivefive.f = 255.0f;

            size_t obtained_vector_length;
            __asm__ volatile(
                "vsetvl %0, %1\n"
                : "=r"(obtained_vector_length)
                : "r"(SIZE - i)
                );
            assert(obtained_vector_length <= SIZE);

            __asm__ volatile(
                "vmsa va0, %0\n"
                "vmsa va1, %1\n"
                "vmsa va2, %2\n"
                "vmsa va3, %3\n"
                "vmss vs0, %4\n"
                "vmss vs1, %5\n"
                "vmss vs2, %6\n"
                "vf 0(%7)\n"
                :
                : "r"(cold + i),
                  "r"(hot + i),
                  "r"(raw + i),
                  "r"(flat + i),
                  "r"(zero.i),
                  "r"(one.i),
                  "r"(twofivefive.i),
                  "r"(&__hwacha_flat)
                : "memory"
                );
        }

        __asm__ volatile(
            "fence\n"
            );
#else
        for (size_t i = 0; i < SIZE; ++i) {
            auto max = hot[i];
            auto min = cold[i];

            auto offset = raw[i] - min;
            auto scale  =  max - min;
            float foffset = offset;
            float fscale = scale;
            auto scaled = foffset / fscale;
            if (scaled > 1.0) scaled = 1.0;
            if (scaled < 0.0) scaled = 0.0;
            flat[i] = ((1 << 16) - 1) * scaled;
        }
#endif

#ifdef HAVE_FFPLAY
        fwriteall(&flat[0],
                  SIZE * sizeof(*flat),
                  ffplay);
#endif

#ifdef WRITE_TO_INTERNAL_MEMORY
        bool matching = true;
        for (int i = 0; i < SIZE; ++i) {
            if ((flat[i] != generate_flat_test::output[i])
                && ((flat[i] + 1) != generate_flat_test::output[i])
                && ((flat[i] - 1) != generate_flat_test::output[i])) {
                printf("ERROR: memory doesn't match at %d: %d <> %d\n",
                       i, flat[i], generate_flat_test::output[i]);
                matching = false;
            }
        }

        if (matching == false)
            abort();
        return 0;
#else
        fwriteall(&flat[0],
                  SIZE * sizeof(*flat),
                  stdout);
        fflush(stdout);
#endif
    }

#ifdef HAVE_FFPLAY
    pclose(ffplay);
#endif

    return 0;
}
