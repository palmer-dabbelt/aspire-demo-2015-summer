#include "colorspace.h++"

#include <stdio.h>
#include <stdlib.h>

#define DELTA (1 / 2560.0)
#define PRINT_INTERMEDIATE_RESULTS

void test(const pixel_hsv& in, const pixel_rgb32& gold)
{
    pixel_rgb32 out = in;

#ifdef PRINT_INTERMEDIATE_RESULTS
    fprintf(stderr, "(%f, %f, %f) => (%u, %u, %u)\n",
            in.h(), in.s(), in.v(),
            out.r(), out.g(), out.b());
#endif

    if (out.r() != gold.r())
        abort();
    if (out.g() != gold.g())
        abort();
    if (out.b() != gold.b())
        abort();
}

void test(double h, double s, double v,
          uint8_t r, uint8_t g, uint8_t b)
{
    return test(
        pixel_hsv(h, s, v),
        pixel_rgb32(r, g, b)
        );
}

void test(const pixel_rgb32& in, const pixel_hsv& gold)
{
    pixel_hsv out = in;

#ifdef PRINT_INTERMEDIATE_RESULTS
    fprintf(stderr, "(%u, %u, %u) => (%f, %f, %f)\n",
            in.r(), in.g(), in.b(),
            out.h(), out.s(), out.v());
#endif

    if (fabs(out.h() - gold.h()) > DELTA)
        abort();
    if (fabs(out.s() - gold.s()) > DELTA)
        abort();
    if (fabs(out.v() - gold.v()) > DELTA)
        abort();
}

void test(uint8_t r, uint8_t g, uint8_t b)
{
    pixel_rgb32 gold(r, g, b);
    pixel_hsv hsv = gold;
    pixel_rgb32 out = hsv;

#ifdef PRINT_INTERMEDIATE_RESULTS
    fprintf(stderr, "(%u, %u, %u) => (%f, %f, %f) => (%u, %u, %u)\n",
            gold.r(), gold.g(), gold.b(),
            hsv.h(), hsv.s(), hsv.v(),
            out.r(), out.g(), out.b()
        );
#endif

    if (abs(out.r() - gold.r()) > 0)
        abort();
    if (abs(out.g() - gold.g()) > 0)
        abort();
    if (abs(out.b() - gold.b()) > 0)
        abort();
}

void test(uint8_t r, uint8_t g, uint8_t b,
          double h, double s, double v)
{
    return test(
        pixel_rgb32(r, g, b),
        pixel_hsv(h, s, v)
        );
}

void test(const pixel_rgb32& gold, const pixel_rgb32& out)
{
#ifdef PRINT_INTERMEDIATE_RESULTS
    fprintf(stderr, "(%u, %u, %u) => (%u, %u, %u)\n",
            gold.r(), gold.g(), gold.b(),
            out.r(), out.g(), out.b());
#endif

    if (out.r() != gold.r())
        abort();
    if (out.g() != gold.g())
        abort();
    if (out.b() != gold.b())
        abort();
}

void test(const pixel_hsv& gold, const pixel_hsv& out)
{
#ifdef PRINT_INTERMEDIATE_RESULTS
    fprintf(stderr, "(%f, %f, %f) => (%f, %f, %f)\n",
            gold.h(), gold.s(), gold.v(),
            out.h(), out.s(), out.v());
#endif

    if (fabs(out.h() - gold.h()) > DELTA)
        abort();
    if (fabs(out.s() - gold.s()) > DELTA)
        abort();
    if (fabs(out.v() - gold.v()) > DELTA)
        abort();
}
