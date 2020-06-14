#pragma once

#include "utils.hpp"
#include "Texture.hpp"
#include "Ray.hpp"
#include "Hit.hpp"

class Object
{
protected:
    Texture *_texture;

public:
    Object() : _texture(nullptr) {}
    explicit Object(Texture *texture) : _texture(texture) {}
    virtual ~Object() = default;

    virtual bool intersect(const Ray &, Hit &) const = 0;

    // @return GUARANTEE that first.x/y/z < second.x/y/z  respectively.
    virtual AABBcord AABB() const = 0;

    Texture *texture() const { return _texture; }
};

#include "Obj_Geometry.hpp"
#include "Obj_Mesh.hpp"
#include "Obj_Group.hpp"
#include "Obj_BVH.hpp"
// #include "Obj_Bezier.hpp" CANCELLED
