#pragma once

#include "SceneParser.hpp"

namespace TEXTURE
{
    Texture textures[] =
        {
            Texture(Vec3(0.9), Texture::Reflect_t::REFR, 1.005),
            Texture(Vec3(0.9), Texture::Reflect_t::DIFF),
            Texture(Vec3(1.0, 0.3, 0.3), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.3, 1.0, 0.3), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.3, 0.3, 1.0), Texture::Reflect_t::DIFF),
            Texture(Vec3(0.2, 0.5, 0.8), Texture::Reflect_t::SPEC),
            Texture(Vec3(0.7, 0.7, 0.7), Texture::Reflect_t::DIFF)};

    Texture t(Vec3(0.9), Texture::Reflect_t::SPEC);
    Texture t1(Vec3(0.99), Texture::Reflect_t::DIFF);
    Texture t2(Vec3(0.999), Texture::Reflect_t::REFR, 1.9);
    Texture t3(Vec3(0.50, 0.75, 0.75), Texture::Reflect_t::DIFF);
} // namespace TEXTURE

SceneParser dragon(
    "dragon",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 90.0, 2000, 1500, 0.),
    new PointLight(Vec3(50, 99.8, 50), Vec3(1.0)),
    {new Plane(Vec3(0, 1, 0), 0., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, -1, 0), 100., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, 0, 1), 0., &(TEXTURE::textures[2])),
     new Plane(Vec3(1, 0, 0), 0., &(TEXTURE::textures[3])),
     new Plane(Vec3(-1, 0, 0), 100., &(TEXTURE::textures[4])),
     new Mesh("obj_models/dragon.100k.obj", Vec3(55, 20, 60), 20, &(TEXTURE::t2)),
     new Mesh("obj_models/dragon.100k.obj", Vec3(45, 20, 40), 20, &(TEXTURE::t3))},
    Vec3(0.));

SceneParser planes2(
    "planes_spec",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 1024, 768, 0.),
    new PointLight(Vec3(50, 99.8, 50), Vec3(1.0)),
    {new Sphere(Vec3(50, 30, 50), 18, &(TEXTURE::textures[1])),
     new Plane(Vec3(0, 1, 0), 0., &(TEXTURE::textures[0])),
     new Plane(Vec3(0, -1, 0), 100., &(TEXTURE::textures[1])),
     new Plane(Vec3(0, 0, 1), 0., &(TEXTURE::textures[2])),
     new Plane(Vec3(1, 0, 0), 0., &(TEXTURE::textures[3])),
     new Plane(Vec3(-1, 0, 0), 100., &(TEXTURE::textures[4]))},
    Vec3(0.));

SceneParser spheres(
    "sphere_as_plane",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 1024, 768, 0.),
    new PointLight(Vec3(50, 99, 50), Vec3(1.0)),
    {new Sphere(Vec3(50, 30, 50), 18, &(TEXTURE::textures[0])),
     new Sphere(Vec3(50, -10000, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(50, 10100, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(-10000, 50, 50), 10001, &(TEXTURE::textures[2])),
     new Sphere(Vec3(10100, 50, 50), 10001, &(TEXTURE::textures[3])),
     new Sphere(Vec3(50, 50, -10000), 10001, &(TEXTURE::textures[4]))},
    Vec3(0.));

SceneParser spheres1(
    "aperture",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 1024, 768, 5.),
    new PointLight(Vec3(50, 99, 50), Vec3(1.0)),
    {new Sphere(Vec3(50, 30, 50), 18, &(TEXTURE::textures[0])),
     new Sphere(Vec3(50, -10000, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(50, 10100, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(-10000, 50, 50), 10001, &(TEXTURE::textures[2])),
     new Sphere(Vec3(10100, 50, 50), 10001, &(TEXTURE::textures[3])),
     new Sphere(Vec3(50, 50, -10000), 10001, &(TEXTURE::textures[4]))},
    Vec3(0.));

SceneParser mesh1(
    "rabbit_200",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 720, 720, 0.),
    new PointLight(Vec3(80, 99, 80), Vec3(0.9)),
    {new Sphere(Vec3(50, -10000, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(50, 10100, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(-10000, 50, 50), 10001, &(TEXTURE::textures[2])),
     new Sphere(Vec3(10100, 50, 50), 10001, &(TEXTURE::textures[3])),
     new Sphere(Vec3(50, 50, -10000), 10001, &(TEXTURE::textures[4])),
     new Mesh("obj_models/bunny_200.obj", Vec3(50, -5, 50), 180., &(TEXTURE::t1))},
    Vec3(0.));
SceneParser mesh1_ap(
    "rabbit_200_ap",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 720, 720, 5.),
    new PointLight(Vec3(80, 99, 80), Vec3(0.9)),
    {new Sphere(Vec3(50, -10000, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(50, 10100, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(-10000, 50, 50), 10001, &(TEXTURE::textures[2])),
     new Sphere(Vec3(10100, 50, 50), 10001, &(TEXTURE::textures[3])),
     new Sphere(Vec3(50, 50, -10000), 10001, &(TEXTURE::textures[4])),
     new Mesh("obj_models/bunny_200.obj", Vec3(50, -5, 50), 180., &(TEXTURE::t1))},
    Vec3(0.));

SceneParser mesh2(
    "triangle_mirrow",
    Camera(Vec3(50, 50, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), 120.0, 720, 720, 0.),
    new PointLight(Vec3(50, 99, 50), Vec3(1.0)),
    {new Sphere(Vec3(50, -10000, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(50, 10100, 50), 10001, &(TEXTURE::textures[1])),
     new Sphere(Vec3(-10000, 50, 50), 10001, &(TEXTURE::textures[2])),
     new Sphere(Vec3(10100, 50, 50), 10001, &(TEXTURE::textures[3])),
     new Sphere(Vec3(50, 50, -10000), 10001, &(TEXTURE::textures[4])),
     new Triangle(Vec3(100, 40, 0), Vec3(55, 0, 0), Vec3(200, 0, 60), &(TEXTURE::t))},
    Vec3(0.));
