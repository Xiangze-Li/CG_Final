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
    Light *_light;
    ObjGroup _group;
    Vec3 _bgColor;

public:
    SceneParser(const Camera &camera,
                Light *light,
                const std::vector<Object *> &objects,
                const Vec3 &backgroundColor)
        : _camera(camera), _light(light), _bgColor(backgroundColor),
          _group(objects.data(), objects.size()) {}
    SceneParser(const SceneParser &) = delete;
    SceneParser(SceneParser &&) = delete;
    ~SceneParser() { delete _light; }

    const Camera &camera() const { return _camera; }
    const Light &light() const { return *_light; }
    const ObjGroup &group() const { return _group; }
    const Vec3 &bg() const { return _bgColor; }
};
