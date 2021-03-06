#include "colorspace.h++"

/* http://www.rapidtables.com/convert/color/hsv-to-rgb.htm */
pixel_rgb32::pixel_rgb32(const pixel_hsv& in)
    : _b(0),
      _g(0),
      _r(0),
      _padding(255)
{
    double s = in.s() / 100.0;
    double v = in.v() / 100.0;
    double hh = in.h() / 60.0;

    double c = v * s;
    double hmod = hh;
    while (fabs(hmod) > 2.0)
        hmod = fabs(hmod - 2.0);
    double x = c * (1 - std::fabs(hmod - 1));
    double m = v - c;

    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    if ((0.0 <= in.h()) && (in.h() < 60.0)) {
        r = c;
        g = x;
        b = 0;
    } else if ((60.0 <= in.h()) && (in.h() < 120.0)) {
        r = x;
        g = c;
        b = 0;
    } else if ((120.0 <= in.h()) && (in.h() < 180.0)) {
        r = 0;
        g = c;
        b = x;
    } else if ((180.0 <= in.h()) && (in.h() < 240.0)) {
        r = 0;
        g = x;
        b = c;
    } else if ((240.0 <= in.h()) && (in.h() < 300.0)) {
        r = x;
        g = 0;
        b = c;
    } else if ((300.0 <= in.h()) && (in.h() < 360.0)) {
        r = c;
        g = 0;
        b = x;
    }

    r += m;
    g += m;
    b += m;

    _r = std::round(r * 255.0);
    _g = std::round(g * 255.0);
    _b = std::round(b * 255.0);
}

/* http://www.rapidtables.com/convert/color/rgb-to-hsv.htm */
pixel_hsv::pixel_hsv(const pixel_rgb32& in)
    : _h(0),
      _s(0),
      _v(0)
{
    double rprime = in.r() / 255.0;
    double gprime = in.g() / 255.0;
    double bprime = in.b() / 255.0;

    double cmax = max(rprime, gprime, bprime);
    double cmin = min(rprime, gprime, bprime);

    double delta = cmax - cmin;
    if (delta == 0.0) delta = 1e-100;

    if (cmax == 0.0) {
        _h = 0.0;
    } else if (cmax == rprime) {
        _h = 60.0 * (((gprime - bprime) / delta) + 0);
    } else if (cmax == gprime) {
        _h = 60.0 * (((bprime - rprime) / delta) + 2);
    } else if (cmax == bprime) {
        _h = 60.0 * (((rprime - gprime) / delta) + 4);
    } else {
        abort();
    }

    if (_h < 0.0)
        _h += 360.0;

    if (cmax > 0.0) {
        _s = delta / cmax * 100.0;
    } else {
        _s = 0.0;
    }

    _v = cmax * 100.0;
}
