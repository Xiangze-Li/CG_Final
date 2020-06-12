#pragma once

#include "utils.hpp"
#include "Texture.hpp"
#include "Ray.hpp"
#include <string>

class Object
{
private:
    Texture _texture;

public:
    Object(const Texture &texture) : _texture(texture) {}

    virtual double intersect(const Ray &) const = 0;
    virtual std::pair<Vec3, Vec3> AABB() const = 0;
    virtual Vec3 norm(const Vec3 &) const = 0;

    const Texture &texture() const { return _texture; }
};

#include "Obj_Geometry.hpp"
// #include "Obj_Bezier.hpp" CANCELLED
