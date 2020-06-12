#pragma once

#include "Vec3.hpp"

class Ray
{
private:
    Vec3 _ori, _dir;

public:
    Ray(const Vec3& ori, const Vec3& dir) : _ori(ori), _dir(dir) {}
    ~Ray() {}

    Vec3 pointAt(double t) const { return (_ori + t * _dir); }
    Vec3 ori() const { return _ori; }
    Vec3 dir() const { return _dir; }
};