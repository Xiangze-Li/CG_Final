#pragma once

#include "Vec3.hpp"
#include "Texture.hpp"
#include "utils.hpp"

class Hit
{
private:
    double _t;
    Texture *_texture;
    Vec3 _normal;
public:
    Hit() : _texture(nullptr), _t(INF), _normal(0.) {}

    float t() const { return _t; }
    Texture *texture() const { return _texture; }
    const Vec3 &normal() const { return _normal; }

    bool set(double t_, Texture* texture_, const Vec3 & normal_)
    {
        _t = t_;
        _texture = texture_;
        _normal = normal_;
        return true;
    }
};
