#pragma once

#include "utils.hpp"
#include "Vec3.hpp"

struct IMGbuffer
{
    double cntr;
    Vec3 color;

    IMGbuffer(double cntr_, const Vec3 &color_) : cntr(cntr_), color(color_) {}
    IMGbuffer() : cntr(0.), color(0.) {}

    void add(const Vec3 &color_, double cntr_ = 1.)
    {
        cntr += cntr_;
        color += color_;
    }
    void reset()
    {
        cntr = 0.;
        color = Vec3(0.);
    }
    IMGbuffer operator+(const IMGbuffer &r) const { return IMGbuffer(cntr + r.cntr, color + r.color); }
    IMGbuffer operator*(const double &r) const { return IMGbuffer(cntr * r, color * r); }
    IMGbuffer operator/(const double &r) const { return IMGbuffer(cntr / r, color / r); }
    IMGbuffer &operator+=(const IMGbuffer &r)
    {
        cntr += r.cntr;
        color += r.color;
        return *this;
    }
    Vec3 getColor() const { return cntr < eps ? color : color / cntr; }
};
