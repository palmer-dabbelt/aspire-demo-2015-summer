/* Copyright 2015 Palmer Dabbelt */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "util.h++"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

template<class T>
T *aligned_alloc(size_t alignment, size_t count __attribute__((unused)), T *out)
{
    while ((((uintptr_t)out) % alignment) != 0)
        out++;
    return (T*)out;
}


template<class T>
T *aligned_alloc(size_t alignment, size_t count)
{
    char *out = new char[sizeof(T) * count * 2];
    return aligned_alloc(alignment, count, out);
}

#ifdef __riscv_hwacha4
/* FIXME: This currently has to be marked as 'extern "C"' because
 * that's the only way I could find to do linkage here. */
extern "C" void __hwacha_memcpy(void);

static uint64_t *hwacha_memcpy_aligned(uint64_t *o, const uint64_t *i, size_t n)
{
    size_t vector_length;
    size_t copied;

    /* This code is going to use a single v64 register to store the
     * intermediate results. */
    __asm__ volatile (
        "vsetcfg 1, 1\n"
        "vsetvl %0, %1\n"
        : "=r"(vector_length)
        : "r"(n)
        );

    /* We don't need a strip-mining preamble here because the memory
     * has to be aligned! */
    copied = 0;
    while ((copied + vector_length) <= n) {
        __asm__ volatile(
            "vmsa va0, %0\n"
            "vmsa va1, %1\n"
            "vf 0(%2)\n"
            :
            : "r"(o), "r"(i), "r"(&__hwacha_memcpy)
            );

        copied += vector_length;
        o += vector_length;
        i += vector_length;
    }

    /* Handle the output strip-mining before any fence, as there won't
     * be any dependencies here. */
    for (size_t ii = copied; ii < n; ++ii)
        o[ii] = i[ii];

    __asm__ volatile(
        "vuncfg\n"
        "fence\n"
        );

    return o;
}

// va0: o
// va1: i
__asm__ volatile(
    "__hwacha_memcpy:\n"
    "  vld vv0, va1\n"
    "  vsd vv0, va0\n"
    "  vstop\n"
    );

static void *hwacha_memcpy(void *o, const void *i, size_t n)
{
    return hwacha_memcpy_aligned((uint64_t *)o,
                                 (uint64_t *)i,
                                 n/sizeof(uint64_t));
}
#else
static void *hwacha_memcpy(void *o, const void *i, size_t n)
{
    return memcpy(o, i, n);
}
#endif

int main(...) {
    char inbuf_storage[BUFFER_SIZE*2];
    char outbuf_storage[BUFFER_SIZE*2];
    auto inbuf  = aligned_alloc<char>(8, BUFFER_SIZE, &inbuf_storage[0]);
    auto outbuf = aligned_alloc<char>(8, BUFFER_SIZE, &outbuf_storage[0]);

    if ((inbuf == nullptr) || (outbuf == nullptr))
        abort();

    for (size_t i = 0; i < BUFFER_SIZE; ++i)
        outbuf[i] = 0;
    for (size_t i = 0; i < BUFFER_SIZE; ++i)
        inbuf[i] = 0;

    while (true) {
        freadall (inbuf,  BUFFER_SIZE, stdin );
        hwacha_memcpy(outbuf, inbuf, BUFFER_SIZE);
        fwriteall(outbuf, BUFFER_SIZE, stdout);
    }
}
