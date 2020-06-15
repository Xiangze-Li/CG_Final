#pragma once

#include "Object.hpp"
#include "utils.hpp"

class BVH_Node : public Object
{
private:
    Object *_child[2];
    Vec3 _aabb[2];
    bool _isLeaf;
    template <int CORD>
    static bool cmp(Object *l, Object *r)
    {
        auto ll = std::get<2>(l->AABB()), rr = std::get<2>(l->AABB());
        return ll[CORD] < rr[CORD];
    }

    static bool intersectWithAABB(const Ray &ray, const AABBcord &aabb)
    {
        double tmin = -eps, tmax = INF;
        Vec3 p0 = std::get<0>(aabb), p1 = std::get<1>(aabb);
        for (size_t DIM = 0; DIM < 3; DIM++)
        {
            double invD = 1. / ray.dir()[DIM];
            double t0 = (p0[DIM] - ray.ori()[DIM]) * invD;
            double t1 = (p1[DIM] - ray.ori()[DIM]) * invD;
            if (invD < 0.)
                std::swap(t0, t1);

            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmax <= tmin)
                return false;
        }
        return true;
    }

public:
    BVH_Node(Object **objs, int size) : Object(nullptr), _isLeaf(false)
    {
        if (size == 1)
        {
            _child[0] = _child[1] = objs[0];
            _isLeaf = true;
            auto tmp = objs[0]->AABB();
            _aabb[0] = std::get<0>(tmp);
            _aabb[1] = std::get<1>(tmp);
            return;
        }

        int DIM = int(rand01() * 3);
        if (DIM == 0)
            std::sort(objs, objs + size, cmp<0>);
        else if (DIM == 1)
            std::sort(objs, objs + size, cmp<1>);
        else
            std::sort(objs, objs + size, cmp<2>);

        if (size == 2)
        {
            _child[0] = objs[0];
            _child[1] = objs[1];
        }
        else
        {
            _child[0] = new BVH_Node(objs, size / 2);
            _child[1] = new BVH_Node(objs + (size / 2), size - size / 2);
        }

        auto tmp0 = _child[0]->AABB(), tmp1 = _child[1]->AABB();
        _aabb[0] = Vec3::mergeMin(std::get<0>(tmp0), std::get<0>(tmp1));
        _aabb[1] = Vec3::mergeMax(std::get<1>(tmp0), std::get<1>(tmp1));
    }

    virtual ~BVH_Node(){
        if (_child[0])
            delete _child[0];
        if (!_isLeaf && _child[1])
            delete _child[1];
    }

    bool intersect(const Ray &ray, Hit &hit) const override
    {
        if (!intersectWithAABB(ray, this->AABB()))
            return false;

        bool result = false;

        if (intersectWithAABB(ray, _child[0]->AABB()))
            result |= _child[0]->intersect(ray, hit);
        if (!_isLeaf && intersectWithAABB(ray, _child[1]->AABB()))
            result |= _child[1]->intersect(ray, hit);

        return result;
    }

    // @return GUARANTEE that first.x/y/z < second.x/y/z  respectively.
    AABBcord AABB() const override { return std::make_tuple(_aabb[0], _aabb[1], (_aabb[0] + _aabb[1]) / 2.); }
};
