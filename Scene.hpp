#pragma once

#include "SceneParser.hpp"

namespace TEXTURE
{
    Texture textures[] =
        {
            Texture(Vec3(0.9), Texture::Reflect_t::DIFF, 1.005),
            Texture(Vec3(0.9), Texture::Reflect_t::DIFF),
            Texture(Vec3(1.0, 0.3, 0.3), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.3, 1.0, 0.3), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.3, 0.3, 1.0), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.2, 0.5, 0.8), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.7, 0.7, 0.7), Texture::Reflect_t::DIFF)};

    Texture t_m(Vec3(0.999), Texture::Reflect_t::SPEC);
    Texture t_m2(Vec3(0.5), Texture::Reflect_t::SPEC);
    Texture t_r(Vec3(0.999), Texture::Reflect_t::REFR, 1.1);
    Texture t_r2(Vec3(0.999, 0.5, 0.5), Texture::Reflect_t::REFR, 1.1);
    Texture t_w(Vec3(0.7));
    Texture t3(Vec3(0.50, 0.75, 0.75), Texture::Reflect_t::DIFF);
} // namespace TEXTURE

SceneParser dragon(
    "dragon",
    Camera(Vec3(50, 30, 99.9), Vec3(0, 0, -1), Vec3(0, 1, 0), 90.0, 1024, 768, 0.),
    new PointLight(Vec3(50, 99.8, 50), Vec3(1.0)),
    {new Plane(Vec3(0, 1, 0), 0., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, -1, 0), 100., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, 0, 1), 0., &(TEXTURE::textures[2])),
     new Plane(Vec3(0, 0, -1), 100, &(TEXTURE::t_w)),
     new Plane(Vec3(1, 0, 0), 0., &(TEXTURE::textures[3])),
     new Plane(Vec3(-1, 0, 0), 100., &(TEXTURE::textures[4])),
     new Mesh("obj_models/dragon.100k.obj", Vec3(55, 20, 60), 20, &(TEXTURE::t_r)),
     new Mesh("obj_models/dragon.100k.obj", Vec3(45, 20, 40), 20, &(TEXTURE::t3))},
    Vec3(0.));

SceneParser dinosaur2(
    "dinosaur",
    Camera(Vec3(0.1, 0.1, 60), Vec3(100, 100, -10), Vec3(0, 0, 1), 90.0, 720, 720, 0.),
    new PointLight(Vec3(80, 80, 99.9), Vec3(1.0)),
    {new Mesh("obj_models/dinosaur.2k.obj", Vec3(50, 50, 50), 0.75, &(TEXTURE::t_r2)),
     new Plane(Vec3(0, 0, 1), 0, &(TEXTURE::textures[2])),
     new Plane(Vec3(0, 0, -1), 100, &(TEXTURE::t_w)),
     new Plane(Vec3(0, -1, 0), 100, &(TEXTURE::t_m)),
     new Plane(Vec3(0, 1, 0), 0, &(TEXTURE::t_w)),
     new Plane(Vec3(1, 0, 0), -100, &(TEXTURE::t_w)),
     new Plane(Vec3(-1, 0, 0), 0, &(TEXTURE::textures[4])),
     new Sphere(Vec3(20, 10, 30), 20, &(TEXTURE::t_r)),
     new Sphere(Vec3(80, 80, 80), 15, &(TEXTURE::t_m2))},
    Vec3(0.));

SceneParser spheres(
    "spheres",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 720, 720, 0.),
    new PointLight(Vec3(0.01, 50, 50), Vec3(1.0)),
    {new Plane(Vec3(0, 1, 0), 0., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, -1, 0), 100., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, 0, 1), 0., &(TEXTURE::textures[2])),
     new Plane(Vec3(1, 0, 0), 0., &(TEXTURE::textures[3])),
     new Plane(Vec3(-1, 0, 0), 100., &(TEXTURE::textures[4])),
     new Sphere(Vec3(50, 30, 50), 15., &TEXTURE::t_w),
     new Sphere(Vec3(30, 70, 30), 20., &TEXTURE::t_m2)},
    Vec3(0.));
