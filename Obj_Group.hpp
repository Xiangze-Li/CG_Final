#pragma once

#include "Object.hpp"
#include <vector>
#include <algorithm>
#include <Vec3.hpp>

class ObjGroup : public Object
{
private:
    std::vector<Object *> _items;
    Vec3 _aabb[2];

public:
    ObjGroup() = default;
    explicit ObjGroup(int size) : Object(), _items(std::vector<Object *>(size, nullptr)) {}
    ~ObjGroup()
    {
        for (auto &ii : _items)
            if (ii)
                delete ii;
    }

    bool intersect(const Ray &ray, Hit &hit) const override
    {
        // OPTIMIZE: AABB acceleration
        bool result = false;

        for (const auto &ii : _items)
            result |= ii->intersect(ray, hit);

        return result;
    }

    std::pair<Vec3,Vec3> AABB() const override
    {
        return std::make_pair(_aabb[0], _aabb[1]);
    }

    std::pair<Vec3, Vec3> AABB() const override
    {
        return std::make_pair(_aabb[0], _aabb[1]);
    }

    bool add(size_t index, Object *obj)
    {
        if (index >= _items.size())
            return false;

        if (_items[index])
            return false;

        _items[index] = obj;
        auto bb = obj->AABB();
        _aabb[0] = Vec3::mergeMin(_aabb[0], bb.first);
        _aabb[1] = Vec3::mergeMax(_aabb[1], bb.second);
        return true;
    }

    size_t size() const { return _items.size(); }
};
