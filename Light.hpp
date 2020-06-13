#pragma once

#include "Vec3.hpp"
#include "Ray.hpp"
#include "utils.hpp"

class Light
{
public:
    Light() = default;
    virtual Ray generateRay() const = 0;
};

class DirectionalLight : public Light
{
private:
    Vec3 _ori, _dir;

public:
    DirectionalLight(const Vec3 &ori, const Vec3 &dir) : _ori(ori), _dir(dir) {}
    Ray generateRay() const override { return Ray(_ori, _dir); }
};

class PointLight : public Light
{
private:
    Vec3 _ori;

public:
    PointLight(const Vec3 &ori) : _ori(ori) {}
    Ray generateRay() const override
    {
        double theta = rand01() * 2. * PI, phi = acos(rand01() * 2. - 1.);
        return Ray(_ori, Vec3(sin(theta) * sin(phi), cos(theta) * sin(phi), cos(phi)));
    }
};
