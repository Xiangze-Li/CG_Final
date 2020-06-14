#pragma once

#include "SceneParser.hpp"

// TODO: 设计场景

namespace TEXTURE
{
    Texture textures[] = {
        Texture()};
} // namespace TEXTURE

SceneParser scene(
    Camera(Vec3(), Vec3(), Vec3(), 0., 1024, 768, 1.),
    {PointLight(Vec3())},
    {new Sphere(Vec3(), 1., &(TEXTURE::textures[0]))},
    Vec3());
