#include "colorspace.h++"

#include <stdio.h>
#include <stdlib.h>

#define DELTA (1 / 2560.0)
#undef PRINT_INTERMEDIATE_RESULTS

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

int main(...)
{
#if 0
    test(0.0, 0.0, 0.0, 0, 0, 0);
    test(0.0, 0.0, 100.0, 255, 255, 255);
    test(0.0, 100.0, 100.0, 255, 0, 0);
    test(120.0, 100.0, 100.0, 0, 255, 0);
    test(240.0, 100.0, 100.0, 0, 0, 255);
    test(60.0, 100.0, 100.0, 255, 255, 0);
    test(180.0, 100.0, 100.0, 0, 255, 255);
    test(300.0, 100.0, 100.0, 255, 0, 255);
    test(0.0, 0.0, 75.0, 191, 191, 191);
    test(0.0, 0.0, 50.0, 128, 128, 128);
    test(0.0, 100.0, 50.0, 128, 0, 0);
    test(60.0, 100.0, 50.0, 128, 128, 0);
    test(120.0, 100.0, 50.0, 0, 128, 0);
    test(300.0, 100.0, 50.0, 128, 0, 128);
    test(180.0, 100.0, 50.0, 0, 128, 128);
    test(240.0, 100.0, 50.0, 0, 0, 128);
#endif

#if 0
    test(0, 0, 0, 0.0, 0.0, 0.0);
    test(255, 255, 255, 0.0, 0.0, 100.0);
    test(255, 0, 0, 0.0, 100.0, 100.0);
    test(0, 255, 0, 120.0, 100.0, 100.0);
    test(0, 0, 255, 240.0, 100.0, 100.0);
    test(255, 255, 0, 60.0, 100.0, 100.0);
    test(0, 255, 255, 180.0, 100.0, 100.0);
    test(255, 0, 255, 300.0, 100.0, 100.0);
    test(192, 192, 192, 0.0, 0.0, 75.294118);
    test(128, 128, 128, 0.0, 0.0, 50.196078);
    test(128, 0, 0, 0.0, 100.0, 50.196078);
    test(128, 128, 0, 60.0, 100.0, 50.196078);
    test(0, 128, 0, 120.0, 100.0, 50.196078);
    test(128, 0, 128, 300.0, 100.0, 50.196078);
    test(0, 128, 128, 180.0, 100.0, 50.196078);
    test(0, 0, 128, 240.0, 100.0, 50.196078);
#endif

#if 0
    for (size_t r = 0; r <= 255; ++r) {
        for (size_t g = 0; g <= 255; ++g) {
            for (size_t b = 0; b <= 255; ++b) {
                test(r, g, b);
            }
        }
    }
#endif

    for (size_t i = 0; i <= 255; ++i) {
        double lambda = i / 255.0;

        auto hi = pixel_rgb32(255, 255, 255);
        pixel_hsv hi_hsv = hi;
        pixel_hsv mul = hi_hsv * lambda;
        pixel_rgb32 mul_rgb = mul;

#ifdef PRINT_INTERMEDIATE_RESULTS
        fprintf(stderr, "(%f, %f, %f)*%f => (%f, %f, %f)\n",
                hi_hsv.h(), hi_hsv.s(), hi_hsv.v(), lambda,
                mul.h(), mul.s(), mul.v());
#endif

        test(mul_rgb, pixel_rgb32(i, i, i));
    }
}
