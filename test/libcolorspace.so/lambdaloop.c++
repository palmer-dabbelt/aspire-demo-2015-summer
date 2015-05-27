#include "harness.h++"

int main(...)
{
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
