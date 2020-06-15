#pragma once

#include "SceneParser.hpp"

// TODO: 设计场景

namespace TEXTURE
{
    Texture textures[] = {
        Texture(Vec3(.7), Texture::Reflect_t::DIFF),
        Texture(Vec3(1.), Texture::Reflect_t::DIFF),
        Texture(Vec3(1, 0, 0), Texture::Reflect_t::DIFF),
        Texture(Vec3(0, 1, 0), Texture::Reflect_t::DIFF),
        Texture(Vec3(0, 0, 1), Texture::Reflect_t::DIFF) };
} // namespace TEXTURE

SceneParser scene(
    "test",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 640, 480, 0.),
    new PointLight(Vec3(50, 99, 50), Vec3(1.5)),
    { new Sphere(Vec3(50, 30, 50), 18, &(TEXTURE::textures[0])),
     new Plane(Vec3(0, 1, 0), 0., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, -1, 0), 100., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, 0, 1), 0., &(TEXTURE::textures[2])),
     new Plane(Vec3(1, 0, 0), 0., &(TEXTURE::textures[3])),
     new Plane(Vec3(-1, 0, 0), 100., &(TEXTURE::textures[4])) },
    Vec3(0.));
