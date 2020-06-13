#pragma once

#include "Object.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

class Mesh : public Object
{
    struct TriangleIndex
    {
        int v[3];
        TriangleIndex() : v{0, 0, 0} {}
        TriangleIndex(int a, int b, int c) : v{a, b, c} {}
        int &operator[](const int &id) { return v[id]; }
        const int &operator[](const int &id) const { return v[id]; }
    };

private:
    std::vector<Vec3> _vtxs;
    std::vector<TriangleIndex> _triangles;
    std::vector<Vec3> _normals;
    Vec3 _aabb[2];

    void computeNormals()
    {
        _normals.resize(_triangles.size());
        for (size_t id = 0; id < _triangles.size(); id++)
        {
            auto &tri = _triangles[id];
            Vec3 a = _vtxs[tri[1]] - _vtxs[tri[0]];
            Vec3 b = _vtxs[tri[2]] - _vtxs[tri[0]];
            _normals[id] = Vec3::cross(a, b).normalized();
        }
    }

public:
    inline Mesh(const char *filename, Texture *texture);

    inline bool intersect(const Ray &, Hit &) const override;

    // @return GUARANTEE that first.x/y/z < second.x/y/z  respectively.
    std::pair<Vec3, Vec3> AABB() const override { return std::make_pair(_aabb[1], _aabb[2]); }
};

inline Mesh::Mesh(const char *filename, Texture *texture) : Object(texture)
{
    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open())
    {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true)
    {
        std::getline(f, line);
        if (f.eof())
        {
            break;
        }
        if (line.size() < 3)
        {
            continue;
        }
        if (line.at(0) == '#')
        {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok)
        {
            Vec3 vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            _aabb[0] = Vec3::mergeMin(_aabb[0], vec);
            _aabb[1] = Vec3::mergeMax(_aabb[1], vec);
            _vtxs.push_back(vec);
        }
        else if (tok == fTok)
        {
            if (line.find(bslash) != std::string::npos)
            {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++)
                {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                }
                _triangles.push_back(trig);
            }
            else
            {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++)
                {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                _triangles.push_back(trig);
            }
        }
        else if (tok == texTok)
        {
            // Shouldn`t use anything more than "v * * *" and "f * * *"
            // Vector2f texcoord;
            // ss >> texcoord[0];
            // ss >> texcoord[1];
        }
    }

    computeNormals();

    f.close();
}

inline bool Mesh::intersect(const Ray &ray, Hit &hit) const
{
    // OPTIMIZE: Acceleration
    bool result = false;
    for (int triId = 0; triId < (int)_triangles.size(); ++triId)
    {
        const TriangleIndex &triIndex = _triangles[triId];
        Triangle triangle(_vtxs[triIndex[0]],
                          _vtxs[triIndex[1]], _vtxs[triIndex[2]], _texture);
        result |= triangle.intersect(ray, hit);
    }
    return result;
}
