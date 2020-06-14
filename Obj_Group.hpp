#pragma once

#include "Obj_BVH.hpp"
#include <vector>
#include <algorithm>
#include <Vec3.hpp>

class ObjGroup : public BVH_Node
{
private:
    // std::vector<Object *> _items;

    Object *const *_objs;
    size_t _size;

public:
    ObjGroup(Object *const *objs, size_t size) : BVH_Node(objs, size), _objs(objs), _size(size) {}
    ~ObjGroup()
    {
        for (size_t i = 0; i < _size; i++)
            delete _objs[i];
    }

    bool intersect(const Ray &ray, Hit &hit) const override
    {
        return BVH_Node::intersect(ray, hit);
    }

    AABBcord AABB() const override
    {
        return BVH_Node::AABB();
    }

    size_t size() const { return _size; }
};
