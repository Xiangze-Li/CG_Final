#pragma once

#include "SceneParser.hpp"

// TODO: 设计场景

namespace TEXTURE
{
    Texture textures[] = {
        Texture(Vec3(.7),Vec3(0.),Texture::Reflect_t::DIFF)};
} // namespace TEXTURE

SceneParser scene(
    "test",
    Camera(Vec3(0.), Vec3(1,0,0), Vec3(0,1,0), 90.0, 1024, 768, 0.),
    new PointLight(Vec3(1,1,1)),
    {new Sphere(Vec3(.3), .2, &(TEXTURE::textures[0]))},
    Vec3(0.));
