#pragma once

#include "Vec3.hpp"
#include "Ray.hpp"
#include "utils.hpp"

class Light
{
private:
    Vec3 _color;

public:
    Light(const Vec3 &color = Vec3(1.2)) : _color(color) {}
    virtual ~Light() = default;
    virtual Ray generateRay() const = 0;
    virtual Vec3 color() const { return _color; }
};

class DirectionalLight : public Light
{
private:
    Vec3 _ori, _dir;

public:
    DirectionalLight(const Vec3 &ori, const Vec3 &dir, const Vec3 &color = Vec3(1.2)) : Light(color), _ori(ori), _dir(dir) {}
    Ray generateRay() const override { return Ray(_ori, _dir); }
};

class PointLight : public Light
{
private:
    Vec3 _ori;

public:
    PointLight(const Vec3 &ori, const Vec3 &color = Vec3(1.2)) : Light(color), _ori(ori) {}
    Ray generateRay() const override
    {
        double theta = rand01() * 2. * PI, phi = acos(rand01() * 2. - 1.);
        return Ray(_ori, Vec3(sin(theta) * sin(phi), cos(theta) * sin(phi), cos(phi)));
    }
};
