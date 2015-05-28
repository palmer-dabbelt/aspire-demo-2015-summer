#ifndef COLORSPACE_HXX
#define COLORSPACE_HXX

#include <stdint.h>
#include <algorithm>
#include <cmath>

template <class T> T max(const T& a, const T& b)
{ return std::max(a, b); }
template <class T> T max(const T& a, const T& b, const T& c)
{ return std::max(a, std::max(b, c)); }

template <class T> T min(const T& a, const T& b)
{ return std::min(a, b); }
template <class T> T min(const T& a, const T& b, const T& c)
{ return std::min(a, std::min(b, c)); }

class pixel_rgb32;
class pixel_hsv;

class pixel_rgb32 {
private:
    uint8_t _b;
    uint8_t _g;
    uint8_t _r;
#ifdef __clang__
    uint8_t _padding __attribute__((unused));
#else
    uint8_t _padding;
#endif

public:
    pixel_rgb32(void)
        {}

    pixel_rgb32(uint8_t r, uint8_t g, uint8_t b)
        : _b(b),
          _g(g),
          _r(r),
          _padding(255)
        {}

    pixel_rgb32(const pixel_hsv& in);

public:
    uint8_t r(void) const { return _r; }
    uint8_t g(void) const { return _g; }
    uint8_t b(void) const { return _b; }
};

class pixel_hsv {
private:
    double _h, _s, _v;

public:
    pixel_hsv(double h, double s, double v)
        : _h(h),
          _s(s),
          _v(v)
        {}

    pixel_hsv(const pixel_rgb32& in);

public:
    double h(void) const { return _h; }
    double s(void) const { return _s; }
    double v(void) const { return _v; }
};

static inline pixel_hsv operator+(const pixel_hsv& a, const pixel_hsv& b)
{
    return pixel_hsv(
        a.h() + b.h(),
        a.s() + b.s(),
        a.v() + b.v()
        );
}

static inline pixel_hsv operator*(const pixel_hsv& a, double b)
{
    return pixel_hsv(
        a.h() * b,
        a.s() * b,
        a.v() * b
        );
}

static inline pixel_hsv operator*(double b, const pixel_hsv& a)
{
    return pixel_hsv(
        a.h() * b,
        a.s() * b,
        a.v() * b
        );
}

#endif
