#pragma once

#include "Object.hpp"
#include <cassert>
#include <vector>
#include <algorithm>

// bool intersect(const Ray &ray, Hit &hit) const override
// std::pair<Vec3, Vec3> AABB() const override

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
        double Ox = ray.ori().x(), Oy = ray.ori().y(), Oz = ray.ori().z();
        double Dx = ray.dir().x(), Dy = ray.dir().y(), Dz = ray.dir().z();

        double t_x0, t_x1, t_y0, t_y1, t_z0, t_z1;
        double near[3], far[3];
        bool closer[3];

        if (abs(Dx) < eps)
        {
            if (Ox < _p0.x() || _p1.x() < Ox)
                return false;

            near[0] = -INF;
            far[0] = INF;
        }
        else
        {
            t_x0 = (_p0.x() - Ox) / Dx;
            t_x1 = (_p1.x() - Ox) / Dx;
            near[0] = ((t_x0 < t_x1 ? t_x0 : t_x1));
            far[0] = ((t_x0 < t_x1 ? t_x1 : t_x0));
            closer[0] = (t_x0 < t_x1 ? true : false);
        }

        if (abs(Dy) < eps)
        {
            if (Oy < _p0.y() || _p1.y() < Oy)
                return false;

            near[1] = -INF;
            far[1] = INF;
        }
        else
        {
            t_y0 = (_p0.y() - Oy) / Dy;
            t_y1 = (_p1.y() - Oy) / Dy;
            near[1] = ((t_y0 < t_y1 ? t_y0 : t_y1));
            far[1] = ((t_y0 < t_y1 ? t_y1 : t_y0));
            closer[1] = (t_y0 < t_y1 ? true : false);
        }

        if (abs(Dz) < eps)
        {
            if (Oz < _p0.z() || _p1.z() < Oz)
                return false;

            near[2] = -INF;
            far[2] = INF;
        }
        else
        {
            t_z0 = (_p0.z() - Oz) / Dz;
            t_z1 = (_p1.z() - Oz) / Dz;
            near[2] = ((t_z0 < t_z1 ? t_z0 : t_z1));
            far[2] = ((t_z0 < t_z1 ? t_z1 : t_z0));
            closer[2] = (t_z0 < t_z1 ? true : false);
        }

        double tFar = *std::min_element(far, far + 2);
        int idxNear = std::max_element(near, near + 2) - near;
        double tNear = near[idxNear];

        if (tFar - tNear < eps || tNear < 0 || tNear >= hit.t())
            return false;

        Vec3 norm;
        if (idxNear == 0)
        {
            if (closer[0])
                norm = Vec3(-1, 0, 0);
            else
                norm = Vec3(1, 0, 0);
        }
        else if (idxNear == 1)
        {
            if (closer[1])
                norm = Vec3(0, -1, 0);
            else
                norm = Vec3(0, 1, 0);
        }
        else if (idxNear == 2)
        {
            if (closer[2])
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

    std::pair<Vec3, Vec3> AABB() const override
    {
        return std::make_pair(_aabb[0], _aabb[1]);
    }
};
