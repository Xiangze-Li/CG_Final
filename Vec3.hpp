#pragma once

#include <cmath>
#include <algorithm>

class Vec3
{
private:
    double _x, _y, _z;

public:
    Vec3(double v = 0.) : _x(v), _y(v), _z(v) {}
    Vec3(double x_, double y_, double z_) : _x(x_), _y(y_), _z(z_) {}
    ~Vec3() {}

    double &x() { return _x; }
    double &y() { return _y; }
    double &z() { return _z; }
    double &operator[](const size_t &index)
    {
        switch (index)
        {
        case 0:
            return _x;
            break;
        case 1:
            return _y;
            break;
        case 2:
            return _z;
            break;
        default:
            throw "Vec3 has only 3 dimentions";
            break;
        }
    }

    double x() const { return _x; }
    double y() const { return _y; }
    double z() const { return _z; }
    double operator[](const size_t &index) const
    {
        switch (index)
        {
        case 0:
            return _x;
            break;
        case 1:
            return _y;
            break;
        case 2:
            return _z;
            break;
        default:
            throw "Vec3 has only 3 dimentions";
            break;
        }
    }

    Vec3 operator-() const { return Vec3(-_x, -_y, -_z); }
    Vec3 operator+(const Vec3 &r) const { return Vec3(_x + r._x, _y + r._y, _z + r._z); }
    Vec3 operator-(const Vec3 &r) const { return Vec3(_x - r._x, _y - r._y, _z - r._z); }
    Vec3 operator*(const Vec3 &r) const { return Vec3(_x * r._x, _y * r._y, _z * r._z); }
    Vec3 operator*(const double &r) const { return Vec3(_x * r, _y * r, _z * r); }
    Vec3 operator/(const Vec3 &r) const { return Vec3(_x / r._x, _y / r._y, _z / r._z); }
    Vec3 operator/(const double &r) const { return Vec3(_x / r, _y / r, _z / r); }
    friend Vec3 operator*(const double &l, const Vec3 &r) { return (r * l); }

    bool operator==(const Vec3 &r) const { return (_x == r._x && _y == r._y && _z == r._z); }
    bool operator!=(const Vec3 &r) const { return !(*this == r); }

    double dot(const Vec3 &r) const { return (_x * r._x + _y * r._y + _z * r._z); }
    Vec3 cross(const Vec3 &r) const { return Vec3(_y * r._z - _z * r._y, _z * r._x - _x * r._z, _x * r._y - _y * r._x); }
    static double dot(const Vec3 &l, const Vec3 &r) { return l.dot(r); }
    static Vec3 cross(const Vec3 &l, const Vec3 &r) { return l.cross(r); }

    double len() const { return sqrt(_x * _x + _y * _y + _z * _z); }
    double squaredLen() const { return (_x * _x + _y * _y + _z * _z); }
    void normalize()
    {
        double norm = len();
        _x /= norm;
        _y /= norm;
        _z /= norm;
    }
    Vec3 normalized() const
    {
        double norm = len();
        return Vec3(_x / norm, _y / norm, _z / norm);
    }

    Vec3 &operator+=(const Vec3 &r)
    {
        _x += r._x;
        _y += r._y;
        _z += r._z;
        return *this;
    }
    Vec3 &operator-=(const Vec3 &r)
    {
        _x -= r._x;
        _y -= r._y;
        _z -= r._z;
        return *this;
    }
    Vec3 &operator*=(const Vec3 &r)
    {
        _x *= r._x;
        _y *= r._y;
        _z *= r._z;
        return *this;
    }
    Vec3 &operator*=(const double &r)
    {
        _x *= r;
        _y *= r;
        _z *= r;
        return *this;
    }
    Vec3 &operator/=(const Vec3 &r)
    {
        _x /= r._x;
        _y /= r._y;
        _z /= r._z;
        return *this;
    }
    Vec3 &operator/=(const double &r)
    {
        _x /= r;
        _y /= r;
        _z /= r;
        return *this;
    }

    Vec3 reflect(const Vec3 &normal) const { return ((*this) - normal * 2. * normal.dot(*this)); }
    Vec3 refract(const Vec3 &normal, double ni, double nr) const
    {
        double cosi = this->normalized().dot(normal);
        double nir = ni / nr;
        double cosr2 = 1. - nir * nir * (1 - cosi * cosi);
        if (cosr2 <= 0)
            return Vec3();
        double cosr = sqrt(cosr2) * (cosi > 0 ? -1 : 1);
        return ((*this) * nir - normal * (nir * cosi + cosr)).normalized();
    }

    static Vec3 lerp(const Vec3 &v0, const Vec3 &v1, double t)
    {
        return ((v1 - v0) * t + v0);
    }

    static Vec3 mergeMin(const Vec3 &v0, const Vec3 &v1)
    {
        return Vec3(std::min(v0._x, v1._x), std::min(v0._y, v1._y), std::min(v0._z, v1._z));
    }
    static Vec3 mergeMax(const Vec3 &v0, const Vec3 &v1)
    {
        return Vec3(std::max(v0._x, v1._x), std::max(v0._y, v1._y), std::max(v0._z, v1._z));
    }

    double L_inf() const { return (_x > _y && _x > _z ? _x : (_y > _z ? _y : _z)); }
};
