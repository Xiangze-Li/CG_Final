#pragma once

#include "Object.hpp"
#include <cassert>
#include <vector>
#include <algorithm>

// double intersect(const Ray &ray) const override {}
// std::pair<Vec3, Vec3> AABB() const override {}
// Vec3 norm(const Vec3 &p) const override {}

class Sphere : public Object
{
private:
    Vec3 _center;
    double _radius;

public:
    Sphere(const Texture &texture, const Vec3 &center, const double &radius) : Object(texture), _center(center), _radius(radius) {}

    double intersect(const Ray &ray) const override
    {
        Vec3 RO = _center - ray.ori();
        double b = ray.dir().dot(RO);
        double d = sqr(b) - RO.dot(RO) + sqr(_radius);

        if (d < 0)
            return INF;
        else
            d = sqrt(d);

        double t = (b - d > eps ? b - d : (b + d > eps ? b + d : -1.));

        if (t < 0)
            return INF;
        else
            return t;
    }
    std::pair<Vec3, Vec3> AABB() const override
    {
        return std::make_pair(_center - Vec3(_radius), _center + Vec3(_radius));
    }
    Vec3 norm(const Vec3 &p) const override
    {
        if (abs((p - _center).len() - _radius) > eps)
            throw "Bad Query: Queried normal with point not on surface";
        return (p - _center).normalized();
    }
};

// The eqation of plane is $ normal (dot) x + d == 0 $
class Plane : public Object
{
private:
    Vec3 _normal;
    double _d;

public:
    Plane(const Texture &texture, const Vec3 &normal, const double &d)
        : Object(texture), _normal(normal), _d(d) {}

    double intersect(const Ray &ray) const override
    {
        double rr = Vec3::dot(_normal, ray.dir());

        if (abs(rr) < eps)
            return INF;
        else
            return ((_d - rr) / rr);
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
    Vec3 norm(const Vec3 &p) const override
    {
        if (abs(_normal.dot(p) + _d) > eps)
            throw "Bad Query: Queried normal with point not on surface";
        return _normal.normalized();
    }
};

// Cubic objects whose edges are PARALLEL to axies (axis-aligned)
// determined by two OPPOSING vertives p0 and p1
class Cube : public Object
{
private:
    Vec3 _p0, _p1;

public:
    Cube(const Texture &texture, const Vec3 &p0, const Vec3 &p1)
        : Object(texture), _p0(p0), _p1(p1) {}

    // Slab-based algorithm for box
    double intersect(const Ray &ray) const override
    {
        Plane pl_0x(Texture(), Vec3(1., 0., 0.), -_p0.x());
        Plane pl_1x(Texture(), Vec3(1., 0., 0.), -_p1.x());
        Plane pl_0y(Texture(), Vec3(0., 1., 0.), -_p0.y());
        Plane pl_1y(Texture(), Vec3(0., 1., 0.), -_p1.y());
        Plane pl_0z(Texture(), Vec3(0., 0., 1.), -_p0.z());
        Plane pl_1z(Texture(), Vec3(0., 0., 1.), -_p1.z());

        double t_0x = pl_0x.intersect(ray);
        double t_0y = pl_0y.intersect(ray);
        double t_0z = pl_0z.intersect(ray);
        double t_1x = pl_1x.intersect(ray);
        double t_1y = pl_1y.intersect(ray);
        double t_1z = pl_1z.intersect(ray);

        std::vector<double> near, far;

        near.emplace_back((t_0x < t_1x ? t_0x : t_1x));
        near.emplace_back((t_0y < t_1y ? t_0y : t_1y));
        near.emplace_back((t_0z < t_1z ? t_0z : t_1z));
        far.emplace_back((t_0x < t_1x ? t_1x : t_0x));
        far.emplace_back((t_0y < t_1y ? t_1y : t_0y));
        far.emplace_back((t_0z < t_1z ? t_1z : t_0z));

        double tNear = *std::max_element(near.begin(), near.end());
        double tFar = *std::min_element(far.begin(), far.end());

        return ((tFar - tNear > eps) ? tNear : INF);
    }
    std::pair<Vec3, Vec3> AABB() const override { return std::make_pair(_p0, _p1); }
    Vec3 norm(const Vec3 &p) const override
    {
        using std::min;
        if (abs(p.x() - _p0.x()) < eps || abs(p.x() - _p1.x() < eps))
            return Vec3((abs(p.x() - min(_p0.x(), _p1.x())) < eps ? -1. : 1.), 0., 0.);
        else if (abs(p.y() - _p0.y()) < eps || abs(p.y() - _p1.y() < eps))
            return Vec3(0., (abs(p.y() - min(_p0.y(), _p1.y())) < eps ? -1. : 1.), 0.);
        else if (abs(p.z() - _p0.z()) < eps || abs(p.z() - _p1.z() < eps))
            return Vec3(0., 0., (abs(p.z() - min(_p0.z(), _p1.z())) < eps ? -1. : 1.));
        else
            throw "Bad Query: Queried normal with point not on surface";
    }
};
