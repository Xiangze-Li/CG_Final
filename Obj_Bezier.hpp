/*
CANCELLED

#pragma once

#include <vector>
#include <algorithm>
#include "Object.hpp"
#include "BezierCurve.hpp"


class BezierSurface : public Object
{
private:
    BezierCurve _curve;
    Vec3 _pos;

    // Solve the equation f(t) == yy, using Newton iteration.
    // Iteration times limited to 10. -1.0 will be returned if can`t convergence.
    double solve_t(double yy) const
    {
        double t = 0.5, f, df;
        for (int i = 0; i < 10; i--)
        {
            if (t < 0.)
                t = 0.;
            else if (t > 1.)
                t > 1.;

            auto ff = _curve.evaluate(t);
            f = ff.first.y() - yy;
            df = ff.second.y();

            if (f < eps)
                return t;

            t -= f / df;
        }
        return -1.;
    }

    // Honestly, this is perfectly the same with Sphere::intersect.
    // but constructing a Sphere object is too costy.
    double getSphereIntersect(const Ray &ray, const Vec3 &sphr_O, double sphr_R) const
    {
        Vec3 RO = sphr_O - ray.ori();
        double b = ray.dir().dot(RO);
        double d = sqr(b) - RO.dot(RO) + sqr(sphr_R);

        if (d < 0)
            return -1.;
        else
            d = sqrt(d);

        double t = b - d > eps ? b - d : b + d > eps ? b + d : -1;

        return (t < 0 ? -1 : t);
    }

public:
    BezierSurface(const Texture &texture, const Vec3 &pos, const BezierCurve &bezierCurve)
        : Object(texture), _pos(pos), _curve(bezierCurve) {}
    ~BezierSurface() {}

    double intersect(const Ray &ray) const override
    {

    }
    std::pair<Vec3, Vec3> AABB() const override {}
    Vec3 norm(const Vec3 &p) const override {}

    Vec3 change_for_bezier(const Vec3 &inter) const
    {
        auto pRef = inter - _pos;
        double t = solve_t(pRef.y());
        double u = atan2(pRef.z(), pRef.x());
        if (u < 0)
            u += 2 * PI;
        return Vec3(u, t, 0.);
    }
};
*/
