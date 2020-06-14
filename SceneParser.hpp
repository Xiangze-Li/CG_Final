#pragma once

#include "Object.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Texture.hpp"
#include <vector>

class SceneParser
{
private:
    Camera _camera;
    std::vector<Light> _lights;
    ObjGroup _group;
    Vec3 _bgColor;

public:
    SceneParser(const Camera &camera,
                const std::vector<Light> &lights,
                const std::vector<Object *> &objects,
                const Vec3 &backgroundColor)
        : _camera(camera), _lights(lights), _bgColor(backgroundColor),
          _group(objects.data(), objects.size()) {}
    SceneParser(const SceneParser &) = delete;
    SceneParser(SceneParser &&) = delete;

    const Camera &camera() const { return _camera; }
    const size_t &lightNum() const { return _lights.size(); }
    const Light &light(const size_t &idx) const { return _lights.at(idx); }
    const ObjGroup &group() const { return _group; }
    const Vec3 &bg() const { return _bgColor; }
};
