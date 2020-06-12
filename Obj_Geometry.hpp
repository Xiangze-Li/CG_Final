#pragma once

#include "Object.hpp"
#include <cassert>
#include <vector>
#include <algorithm>

// bool intersect(const Ray &ray, Hit &hit) const override

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
    std::pair<Vec3, Vec3> AABB() const override
    {
        return std::make_pair(_center - Vec3(_radius), _center + Vec3(_radius));
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

        if (abs(rr) < eps)
            return false;

        double tt = ((_d - rr) / rr);

        if (tt < 0 || tt >= hit.t())
            return false;

        return hit.set(tt, _texture, _normal.normalized());
    }
    std::pair<Vec3, Vec3> AABB() const override
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

        return std::make_pair(p0, p1);
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
        // FIXME: this function will return INF when the ray is axis-align, even if there is accully an intersection.
        /*
        if (abs(ray.dir().x()) < eps && abs(ray.dir().y()) < eps)
        {
        }
        else if (abs(ray.dir().x()) < eps && abs(ray.dir().z()) < eps)
        {
        }
        else if (abs(ray.dir().y()) < eps && abs(ray.dir().z()) < eps)
        {
        }
        */

        double t_0x = getPlaneIntersect(ray, Vec3(1, 0, 0), -_p0.x());
        double t_1x = getPlaneIntersect(ray, Vec3(1, 0, 0), -_p1.x());
        double t_0y = getPlaneIntersect(ray, Vec3(0, 1, 0), -_p0.y());
        double t_1y = getPlaneIntersect(ray, Vec3(0, 1, 0), -_p1.y());
        double t_0z = getPlaneIntersect(ray, Vec3(0, 0, 1), -_p0.z());
        double t_1z = getPlaneIntersect(ray, Vec3(0, 0, 1), -_p1.z());

        std::vector<double> near, far;

        near.emplace_back((t_0x < t_1x ? t_0x : t_1x));
        near.emplace_back((t_0y < t_1y ? t_0y : t_1y));
        near.emplace_back((t_0z < t_1z ? t_0z : t_1z));
        far.emplace_back((t_0x < t_1x ? t_1x : t_0x));
        far.emplace_back((t_0y < t_1y ? t_1y : t_0y));
        far.emplace_back((t_0z < t_1z ? t_1z : t_0z));

        double tFar = *std::min_element(far.begin(), far.end());
        auto idxNear = std::max_element(near.begin(), near.end()) - near.begin();
        double tNear = near[idxNear];
        // return ((tFar - tNear > eps) ? tNear : INF);
        if (tFar - tNear < eps || tNear < 0 || tNear >= hit.t())
            return false;

        Vec3 norm;
        if (idxNear == 0)
        {
            if (abs(ray.pointAt(tNear).x() - _p0.x()) < eps)
                norm = Vec3(-1, 0, 0);
            else
                norm = Vec3(1, 0, 0);
        }
        else if (idxNear == 1)
        {
            if (abs(ray.pointAt(tNear).y() - _p0.y()) < eps)
                norm = Vec3(0, -1, 0);
            else
                norm = Vec3(0, 1, 0);
        }
        else if (idxNear == 2)
        {
            if (abs(ray.pointAt(tNear).z() - _p0.z()) < eps)
                norm = Vec3(0, 0, -1);
            else
                norm = Vec3(0, 0, 1);
        }

        return hit.set(tNear, _texture, norm);
    }
    std::pair<Vec3, Vec3> AABB() const override { return std::make_pair(_p0, _p1); }
};

class Triangle : public Object
{
private:
    Vec3 _vtx[3], _normal;
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
    }
    bool intersect(const Ray &ray, Hit &hit) const override
    {
        double rr = Vec3::dot(_normal, ray.dir());

        if (abs(rr) < eps)
            return false;

        double tt = ((_d - rr) / rr);

        if (tt < 0 || tt >= hit.t())
            return false;

        if (inside(ray.pointAt(tt)))
            return hit.set(tt, _texture, _normal);
        else
            return false;
    }
};
