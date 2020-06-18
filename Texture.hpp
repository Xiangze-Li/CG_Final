#pragma once

#include <utility>
#include "Vec3.hpp"

class Texture
{
public:
    enum class Reflect_t
    {
        DIFF, // DIFFuse
        SPEC, // SPECular
        REFR  // REFRactive
    };
    using Feature = std::pair<Reflect_t, Vec3>;

private:
    Vec3 _color;
    Reflect_t _refl;
    double _brdf;

public:
    Texture(Vec3 color = Vec3(1.), Reflect_t refl = Texture::Reflect_t::DIFF, double brdf = 1.5)
        : _color(color), _refl(refl), _brdf(brdf) {}

    Feature getFeature() const
    {
        return std::make_pair(_refl, _color);
    }

    double brdf() const { return _brdf; }
};
