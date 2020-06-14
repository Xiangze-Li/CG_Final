#pragma once

#include "Object.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

class Mesh : public Object
{
private:
    std::vector<Object *> _triangles;
    BVH_Node *bvhTree;

    inline void loadFromFile(const char *filename);

public:
    Mesh(const char *filename, Texture *texture)
    {
        loadFromFile(filename);
        bvhTree = new BVH_Node(_triangles.data(), _triangles.size());
    }
    ~Mesh()
    {
        for (auto& i:_triangles)
            delete i;
    }

    bool intersect(const Ray &ray, Hit &hit) const override { return bvhTree->intersect(ray, hit); }

    AABBcord AABB() const override { return bvhTree->AABB(); }
};

/* inline bool Mesh::intersect(const Ray &ray, Hit &hit) const
{
    // OPTIMIZE: Acceleration
    bool result = false;
    for (const auto &tri : _triangles)
    {
        result |= tri->intersect(ray, hit);
    }
    return result;
}
 */
inline void Mesh::loadFromFile(const char *filename)
{
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
    std::vector<Vec3> _vtxs;
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
            double vec[3];
            ss >> vec[0] >> vec[1] >> vec[2];
            _vtxs.emplace_back(vec[0], vec[1], vec[2]);
        }
        else if (tok == fTok)
        {
            if (line.find(bslash) != std::string::npos)
            {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                int trig[3];
                facess >> tok;
                for (int ii = 0; ii < 3; ii++)
                {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                }
                _triangles.push_back(new Triangle(_vtxs[trig[0]], _vtxs[trig[1]], _vtxs[trig[2]], _texture));
            }
            else
            {
                int trig[3];
                for (int ii = 0; ii < 3; ii++)
                {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                _triangles.push_back(new Triangle(_vtxs[trig[0]], _vtxs[trig[1]], _vtxs[trig[2]], _texture));
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

    f.close();
}
