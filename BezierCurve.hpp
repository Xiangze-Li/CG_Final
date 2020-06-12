#pragma once

#include <vector>
#include <algorithm>
#include "Vec3.hpp"

class BezierCurve
{
private:
    std::vector<Vec3> _controls;
    int _degree;

public:
    explicit BezierCurve(std::vector<Vec3> controls, int degree = -1)
        : _controls(std::move(controls))
    {
        if (degree == -1)
            _degree = controls.size() - 1;
        else
        {
            if (degree != controls.size() - 1)
                throw "Bad Bezier Controls: Number of control points should be degree + 1";
            else
                _degree = degree;
        }
    }
    ~BezierCurve() {}

    // De Casteljau algorithm for Bezier curve evalutaion
    // @return: std::pair<Vec3, Vec3>, first = f(t), second = f'(t).normalized()
    std::pair<Vec3, Vec3> evaluate(double t) const
    {
        auto ctrls = _controls;
        for (int i = 0; i < _degree - 1; i++)
        {
            for (int j = 0; j < _degree - i; j++)
                ctrls[j] = Vec3::lerp(ctrls[j], ctrls[j + 1], t);
        }
        Vec3 tangent = (ctrls[1] - ctrls[0]).normalized();
        Vec3 vertex = Vec3::lerp(ctrls[0], ctrls[1], t);
        return std::make_pair(vertex, tangent);
    }
};