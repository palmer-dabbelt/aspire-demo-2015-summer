#ifndef HWACHA_HXX
#define HWACHA_HXX

#include <stdint.h>

#ifndef __riscv_hwacha4
void flatten(uint8_t *flat, uint16_t *raw,
    uint16_t *hot, uint16_t *cold)
{
    for (size_t i = 0; i < SIZE; ++i) {
        auto max = hot[i];
        auto min = cold[i];

        auto scaled = (float)(raw[i] - min) / (max - min);
        if (scaled > 1.0) scaled = 1.0;
        if (scaled < 0.0) scaled = 0.0;
        flat[i] = ((1 << 8) - 1) * scaled;
    }
}
#endif

#ifdef __riscv_hwacha4
void flatten(uint8_t *flat, uint16_t *raw,
    uint16_t *hot, uint16_t *cold)
{
  float one = 1.0f;
  float zero = 0.0f;
  asm volatile (
  "vsetcfg  9, 1\n"
  "li a5, 208 * 156\n"
  "li a4, ((1 << 8) - 1)\n"
  "fcvt.s.w fa1, a4\n"
  "fmv.x.s a4, fa1\n"
  "vmss vs1, a4\n"
  "fmv.x.s a4, %4\n"
  "vmss vs2, a4\n"
  "fmv.x.s a4, %5\n"
  "vmss vs3, a4\n"
  "lui a4, %%hi(vtcode%=)\n"
  "stripmine%=:\n"
  "vsetvl t0, a5\n"
  "sb x0, 0(%0)\n"
  "lhu a6, 0(%1)\n"
  "lhu a6, 0(%2)\n"
  "lhu a6, 0(%3)\n"
  "vmsa va0, %0\n"
  "vmsa va1, %1\n"
  "vmsa va2, %2\n"
  "vmsa va3, %3\n"
  "slli t1, t0, 1\n"
  "add %0, %0, t0\n"
  "add %1, %1, t1\n"
  "add %2, %2, t1\n"
  "add %3, %3, t1\n"
  "sb x0, 0(%0)\n"
  "lhu a6, 0(%1)\n"
  "lhu a6, 0(%2)\n"
  "lhu a6, 0(%3)\n"
  "vf  %%lo(vtcode%=)(a4)\n"
  "sub a5, a5, t0\n"
  "bnez a5, stripmine%=\n"
  "fence\n"
  "j 1f\n"
  ".align 3\n"
  "vtcode%=:\n"
  "vlhu vv1, va1\n"
  "vlhu vv2, va2\n"
  "vlhu vv3, va3\n"
  "vsub vv4, vv1, vv3\n"
  "vsub vv5, vv2, vv3\n"
  "vfcvt.s.l vv6, vv4\n"
  "vfcvt.s.l vv7, vv5\n"
  "vfdiv.s vv8, vv6, vv7\n"
  //scaled in vv8 pre-branch
  "vfmin.s vv8, vv8, vs2\n"
  "vfmax.s vv8, vv8, vs3\n"
  // scaled now clamped to 0.0 to 1.0 in vv8
  "vfmul.s vv8, vv8, vs1\n"
  "vfcvt.w.s vv8, vv8\n"
  "vsb vv8, va0\n"
  "vstop\n"
  "1:\n"
  : "+r" (flat)
  : "r" (raw), "r" (hot), "r" (cold), "f" (one), "f" (zero)
  : "a4", "a5", "a6");
}
#endif
#endif
