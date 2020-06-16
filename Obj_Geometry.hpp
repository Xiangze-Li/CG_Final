#pragma once

#include "Object.hpp"
#include <cassert>
#include <vector>
#include <algorithm>

class Sphere : public Object
{
private:
    Vec3 _center;
    double _radius;

public:
    Sphere(const Vec3 &center, const double &radius, Texture *texture) : Object(texture), _center(center), _radius(radius) {}

    bool intersect(const Ray &ray, Hit &hit) const override
    {
        Vec3 vec_OC = _center - ray.ori();
        double len_OH = ray.dir().normalized().dot(vec_OC);
        if (len_OH < 0)
            return false;
        double len_CH = sqrt(vec_OC.squaredLen() - sqr(len_OH));
        if (len_CH > _radius)
            return false;
        double len_PH = sqrt(sqr(_radius) - sqr(len_CH));
        double tt = (len_OH - len_PH) / ray.dir().len();
        if (tt > hit.t())
            return false;

        return hit.set(tt, _texture, (ray.pointAt(tt) - _center).normalized());
    }
    AABBcord AABB() const override
    {
        return std::make_tuple(_center - Vec3(_radius + eps), _center + Vec3(_radius + eps), _center);
    }
};

// The eqation of plane is $ normal (dot) x + d == 0 $
class Plane : public Object
{
private:
    Vec3 _normal;
    double _d;

public:
    Plane(const Vec3 &normal, const double &d, Texture *texture)
        : Object(texture), _normal(normal), _d(d) {}

    bool intersect(const Ray &ray, Hit &hit) const override
    {
        double rr = Vec3::dot(_normal, ray.dir());

        double tt = ((-_d - _normal.dot(ray.ori())) / rr);

        if (tt < 0 || tt >= hit.t())
            return false;

        // 希望这样能够解决自遮挡问题
        while (rr * (_normal.dot(ray.pointAt(tt)) + _d) > -eps)
        {
            tt -= eps;
        }

        return hit.set(tt, _texture, _normal.normalized());
    }
    AABBcord AABB() const override
    {
        Vec3 p0(-10000.0), p1(10000.0);

        if (abs(_normal.x()) < eps && abs(_normal.y()) < eps)
        {
            p0.z() = -(_d / _normal.z()) - eps;
            p1.z() = -(_d / _normal.z()) + eps;
        }
        else if (abs(_normal.x()) < eps && abs(_normal.z()) < eps)
        {
            p0.y() = -(_d / _normal.y()) - eps;
            p1.y() = -(_d / _normal.y()) + eps;
        }
        else if (abs(_normal.y()) < eps && abs(_normal.z()) < eps)
        {
            p0.x() = -(_d / _normal.x()) - eps;
            p1.x() = -(_d / _normal.x()) + eps;
        }

        return std::make_tuple(p0, p1, (p0 + p1) / 2.);
    }
};

// Cubic objects whose edges are PARALLEL to axies (axis-aligned)
// determined by two OPPOSING vertives p0 and p1
class Cube : public Object
{
private:
    Vec3 _p0, _p1;
    double getPlaneIntersect(const Ray &ray, const Vec3 &pln_N, double pln_d) const
    {
        double rr = Vec3::dot(pln_N, ray.dir());
        return ((pln_d - rr) / rr);
    }

public:
    Cube(const Vec3 &p0, const Vec3 &p1, Texture *texture)
        : Object(texture)
    {
        _p0 = Vec3::mergeMin(p0, p1);
        _p1 = Vec3::mergeMax(p0, p1);
    }

    // Slab-based algorithm for box
    bool intersect(const Ray &ray, Hit &hit) const override
    {
        double tmin = -eps, tmax = INF;
        for (size_t DIM = 0; DIM < 3; DIM++)
        {
            double invD = 1. / ray.dir()[DIM];
            double t0 = (_p0[DIM] - ray.ori()[DIM]) * invD;
            double t1 = (_p1[DIM] - ray.ori()[DIM]) * invD;
            if (invD < 0.)
                std::swap(t0, t1);

            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmax <= tmin)
                return false;
        }

        if (tmin < 0 || tmin >= hit.t())
            return false;

        Vec3 norm(0.), inter(ray.pointAt(tmin));
        for (size_t DIM = 0; DIM < 3; DIM++)
        {
            if (abs(_p0[DIM] - inter[DIM]) < eps)
            {
                norm[DIM] = -1;
                break;
            }
            else if (abs(_p1[DIM] - inter[DIM]) < eps)
            {
                norm[DIM] = 1;
                break;
            }
        }
        assert(norm.squaredLen() > eps);
        return hit.set(tmin, _texture, norm);
    }

    AABBcord AABB() const override { return std::make_tuple(_p0 - Vec3(eps), _p1 + Vec3(eps), (_p0 + _p1) / 2.); }
};

class Triangle : public Object
{
private:
    Vec3 _vtx[3], _normal;
    Vec3 _aabb[2];
    double _d;
    bool inside(const Vec3 &P) const
    {
        return (_normal.dot(Vec3::cross(_vtx[0] - P, _vtx[1] - P)) > 0 &&
                _normal.dot(Vec3::cross(_vtx[1] - P, _vtx[2] - P)) > 0 &&
                _normal.dot(Vec3::cross(_vtx[2] - P, _vtx[0] - P)) > 0);
    }

public:
    Triangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, Texture *texture)
        : Object(texture), _vtx{a, b, c}
    {
        _normal = Vec3::cross(b - a, c - a).normalized();
        _d = -_normal.dot(a);
        _aabb[0] = Vec3::mergeMin(a, Vec3::mergeMin(b, c)) - Vec3(eps);
        _aabb[1] = Vec3::mergeMax(a, Vec3::mergeMax(b, c)) + Vec3(eps);
    }

    bool intersect(const Ray &ray, Hit &hit) const override
    {
        double rr = Vec3::dot(_normal, ray.dir());
        // if (abs(rr) < eps)
        //     return false;

        double tt = ((-_d - _normal.dot(ray.ori())) / rr);

        if (tt < 0 || tt >= hit.t())
            return false;

        if (!inside(ray.pointAt(tt)))
            return false;

        // 希望这样能够解决自遮挡问题
        while (rr * (_normal.dot(ray.pointAt(tt)) + _d) > -eps)
        {
            tt -= eps;
        }

        return hit.set(tt, _texture, _normal);
    }

    AABBcord AABB() const override
    {
        return std::make_tuple(_aabb[0], _aabb[1], (_aabb[0] + _aabb[1]) / 2.);
    }
};
