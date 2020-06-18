#pragma once

#include "Object.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Texture.hpp"
#include <vector>
#include <string>

class SceneParser
{
private:
    std::string _name;
    Camera _camera;
    Light *_light;
    ObjGroup _group;
    Vec3 _bgColor;

public:
    SceneParser(const std::string &name,
                const Camera &camera,
                Light *light,
                std::vector<Object *> &&objects,
                const Vec3 &backgroundColor)
        : _name(name), _camera(camera),
          _light(light), _bgColor(backgroundColor),
          _group(objects.data(), objects.size()) {}
    SceneParser(const SceneParser &) = delete;
    SceneParser(SceneParser &&) = delete;
    ~SceneParser() { delete _light; }

    const Camera &camera() const { return _camera; }
    const Light &light() const { return *_light; }
    const ObjGroup &group() const { return _group; }
    const Vec3 &bg() const { return _bgColor; }
    const std::string &name() const { return _name; }
};
