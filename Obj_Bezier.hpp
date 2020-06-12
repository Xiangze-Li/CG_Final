#pragma once

#include <vector>
#include <algorithm>
#include "Object.hpp"
#include "BezierCurve.hpp"

// FIXME: NOT COMPLETE
class BezierSurface : public Object
{
private:
    /* data */
public:
    BezierSurface(const Texture &texture /* */) : Object(texture) {}
    ~BezierSurface() {}

    double intersect(const Ray &ray) const override {}
    std::pair<Vec3, Vec3> AABB() const override {}
    Vec3 norm(const Vec3 &p) const override {}

    Vec3 change_for_bezier(const Vec3 &x);
};
