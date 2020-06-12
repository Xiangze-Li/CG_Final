#pragma once

#include "Texture.hpp"
#include "scene.hpp"
#include "KDtree.hpp"
#include <utility>
#include <vector>

std::pair<int, double> findIntersect(const Ray &ray)
// TODO: AABB
{
    double t = INF;
    int id = -1;
    for (int i = 0; i < scene_num; i++)
    {
        auto tmpIntersect = scene[i]->intersect(ray);
        if (tmpIntersect < t)
        {
            t = tmpIntersect;
            id = i;
        }
    }
    return std::make_pair(id, t);
}

std::vector<SPPMNode> sppmBacktrace(const Ray &ray, int depth, int index, Vec3 pref = Vec3(1.), double prob = 1.)
{
    std::vector<SPPMNode> result;

    if (pref.len() < eps || prob < eps)
        return result;

    auto intersection = findIntersect(ray);
    if (intersection.first == -1)
        return result;

    int into = 0;
    auto obj = scene[intersection.first];
    const Texture &texture = obj->texture();
    Vec3 inter = ray.pointAt(intersection.second);
    auto feature = getFeature(obj, texture, inter);
    Vec3 color = feature.second, normal = obj->norm(inter), nl = normal;
    if (normal.dot(ray.dir()) > 0)
        nl = -nl;
    else
        into = 1;

    double p = color.L_inf();
    if (p < eps)
        return result;

    depth++;

    if (depth > 5)
    {
        if (rand01() < p)
            color /= p;
        else
            return result;
    }

    Ray reflectRay = Ray(inter, ray.dir().reflect(nl));

    if (texture.filename() == "vase.png")
    {
        result.push_back(SPPMNode(inter, pref * color, nl, 1., index, prob * 0.9));
        auto tmp = sppmBacktrace(reflectRay, depth, index, pref * color, prob * 0.1);
        result.insert(result.end(), tmp.begin(), tmp.end());
        return result;
    }

    switch (feature.first)
    {
    case Texture::Reflect_t::DIFF:
        result.push_back(SPPMNode(inter, pref * color, nl, 1., index, prob));
        break;
    case Texture::Reflect_t::SPEC:
    {
        auto tmp = sppmBacktrace(reflectRay, depth, index, pref * color, prob);
        result.insert(result.end(), tmp.begin(), tmp.end());
        break;
    }
    case Texture::Reflect_t::REFR:
    {
        Vec3 refractDir = ray.dir().refract(normal, into ? 1. : texture.brdf(), into ? texture.brdf() : 1.);
        Ray refractRay = Ray(inter, refractDir);
        if (refractDir.squaredLen() < eps) //
            return sppmBacktrace(reflectRay, depth, index, pref * color, prob);

        double a = texture.brdf() - 1, b = texture.brdf() + 1;
        double R0 = sqr(a) / sqr(b);
        double c = 1. - (into ? -ray.dir().dot(nl) : refractDir.dot(normal));
        double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re;
        double P = 0.25 + 0.5 * Re, RP = Re / P, TP = Tr / (1 - P);

        if (depth > 2)
        {
            if (rand01() < P)
            {
                auto tmp = sppmBacktrace(reflectRay, depth, index, pref * color, prob * RP);
                result.insert(result.end(), tmp.begin(), tmp.end());
            }
            else
            {
                auto tmp = sppmBacktrace(refractRay, depth, index, pref * color, prob * TP);
                result.insert(result.end(), tmp.begin(), tmp.end());
            }
        }
        else
        {
            auto tmp = sppmBacktrace(reflectRay, depth, index, pref * color, prob * Re);
            result.insert(result.end(), tmp.begin(), tmp.end());
            auto tmp1 = sppmBacktrace(refractRay, depth, index, pref * color, prob * Tr);
            result.insert(result.end(), tmp1.begin(), tmp1.end());
        }
    }
    }
    return result;
}

void sppmForward(const Ray &ray, int depth, const Vec3 &prevColor, IMGbuffer *img, KDtree *kdt, double prob = 1.)
{
    if (prevColor.L_inf() < eps)
        return;

    auto intersection = findIntersect(ray);
    if (intersection.first == -1)
        return;

    int into = 0;
    Object *obj = scene[intersection.first];
    const Texture &texture = obj->texture();
    Vec3 inter = ray.pointAt(intersection.second);
    auto feature = getFeature(obj, texture, inter);

    Vec3 color = feature.second, normal = obj->norm(inter), nl = normal;
    if (normal.dot(ray.dir()) > 0)
        nl = -nl;
    else
        into = 1;

    double p = color.L_inf();
    if (p < eps)
    {
        kdt->query(SPPMNode(inter, prevColor, nl), img);
        return;
    }

    depth++;

    if (depth > 5)
    {
        if (rand01() < p)
            color /= p;
        else
        {
            kdt->query(SPPMNode(inter, prevColor, nl), img);
            return;
        }
    }

    if (feature.first == Texture::Reflect_t::DIFF)
    {
        kdt->query(SPPMNode(inter, prevColor, nl), img);
        double r1 = 2 * PI * rand01(), r2 = rand01(), r2s = sqrt(r2);
        Vec3 u = (abs(nl.x()) > 1. ? Vec3(0., 1., 0.) : Vec3(1., 0., 0.)).cross(nl).normalized();
        Vec3 v = nl.cross(u);
        Vec3 dir = (u * cos(r1) * r2s + v * sin(r1) * r2s + nl * sqrt(1 - r2)).normalized();
        return sppmForward(Ray(inter, dir), depth, prevColor * color, img, kdt, prob);
    }
    else
    {
        Ray reflectRay = Ray(inter, ray.dir().reflect(nl));

        if (feature.first == Texture::Reflect_t::SPEC)
        {
            if (texture.filename() == "vase.png")
                kdt->query(SPPMNode(inter, prevColor, nl), img);
            return sppmForward(reflectRay, depth, color * prevColor, img, kdt, prob);
        }
        else
        {
            Vec3 refractDir = ray.dir().refract(normal, into ? 1. : texture.brdf(), into ? texture.brdf() : 1.);
            Ray refractRay = Ray(inter, refractDir);
            if (refractDir.squaredLen() < eps) //
                return sppmForward(reflectRay, depth, prevColor * color, img, kdt, prob);

            double a = texture.brdf() - 1, b = texture.brdf() + 1;
            double R0 = sqr(a) / sqr(b);
            double c = 1. - (into ? -ray.dir().dot(nl) : refractDir.dot(normal));
            double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re;
            double P = 0.25 + 0.5 * Re, RP = Re / P, TP = Tr / (1 - P);

            if (depth > 2)
            {
                if (rand01() < P)
                    return sppmForward(reflectRay, depth, color * prevColor, img, kdt, prob * RP);
                else
                    return sppmForward(refractRay, depth, color * prevColor, img, kdt, prob * TP);
            }
            else
            {
                sppmForward(reflectRay, depth, color * prevColor, img, kdt, prob * Re);
                return sppmForward(refractRay, depth, color * prevColor, img, kdt, prob * Tr);
            }
        }
    }
}
