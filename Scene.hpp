#pragma once

#include "SceneParser.hpp"

// TODO: 设计场景

namespace TEXTURE
{
    Texture textures[] =
        {
            Texture(Vec3(.9), Texture::Reflect_t::REFR),
            Texture(Vec3(1.), Texture::Reflect_t::DIFF),
            Texture(Vec3(1.0, 0.3, 0.3), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.3, 1.0, 0.3), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.3, 0.3, 1.0), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.2, 0.5, 0.8), Texture::Reflect_t::SPEC)};
} // namespace TEXTURE

SceneParser scene1(
    "test",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 640, 480, 0.),
    new PointLight(Vec3(50, 99.8, 50), Vec3(1.5)),
    {new Sphere(Vec3(50, 30, 50), 18, &(TEXTURE::textures[0])),
     new Plane(Vec3(0, 1, 0), 0., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, -1, 0), 100., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, 0, 1), 0., &(TEXTURE::textures[2])),
     new Plane(Vec3(1, 0, 0), 0., &(TEXTURE::textures[3])),
     new Plane(Vec3(-1, 0, 0), 100., &(TEXTURE::textures[4]))},
    Vec3(0.));

SceneParser scene(
    "sphere_as_plane",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 1024, 768, 0.),
    new PointLight(Vec3(50, 99, 50), Vec3(1.0)),
    {new Sphere(Vec3(50, 30, 50), 18, &(TEXTURE::textures[0])),
     new Sphere(Vec3(50, -10000, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(50, 10100, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(-10000, 50, 50), 10001, &(TEXTURE::textures[2])),
     new Sphere(Vec3(10100, 50, 50), 10001, &(TEXTURE::textures[3])),
     new Sphere(Vec3(50, 50, -10000), 10001, &(TEXTURE::textures[4])),
     new Cube(Vec3(5.), Vec3(35.), &(TEXTURE::textures[5]))},
    Vec3(0.));
