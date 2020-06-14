#pragma once

#include "Vec3.hpp"
#include "Ray.hpp"
#include "utils.hpp"
#include <cmath>

class Camera
{
private:
    Vec3 _center, _dir, _up, _right;
    int _width, _height;
    double _dist;
    double _aperture;

    Vec3 _revArray[3];
    Vec3 toGlobal(const Vec3 &local) const
    {
        return Vec3(
            _revArray[0].dot(local),
            _revArray[1].dot(local),
            _revArray[2].dot(local));
    }
    void buildRevArray()
    {
        _revArray[0] = Vec3(_right[0], _up[0], _dir[0]);
        _revArray[1] = Vec3(_right[1], _up[1], _dir[1]);
        _revArray[2] = Vec3(_right[2], _up[2], _dir[2]);
    }

public:
    // @param: angleX should be given in **degree(s)**
    Camera(const Vec3 &center, const Vec3 &dir, const Vec3 &up, double angleX, int width, int height, double aperture)
        : _center(center), _dir(dir.normalized()),
          _right(_dir.cross(up).normalized()),
          _width(width), _height(height), _aperture(aperture)
    {
        angleX = (angleX / 360.) * 2 * PI;
        _up = _right.cross(_dir).normalized();
        _dist = (_width / 2.) / tan(angleX / 2.);
        buildRevArray();
    }

    Ray generateRay(double x, double y) const
    {
        double rr = sqrt(rand01()) * _aperture;
        double theta = rand01() * 2 * PI;

        Vec3 newOri = Vec3(rr * cos(theta), rr * sin(theta), 0.);

        Vec3 dir = Vec3(x - _width / 2., y - _height / 2., _dist) - newOri;
        dir.normalize();

        return Ray(_center + toGlobal(newOri), toGlobal(dir));
    }

    int width() const { return _width; }
    int height() const { return _height; }
};
