// FIXME: 需要使用新的交点返回方式重构求交
// FIXME: 需要加入传入场景 Object 的参数
#pragma once

#include "Texture.hpp"
#include "KDtree.hpp"
#include "Object.hpp"
#include <utility>
#include <vector>

std::vector<SPPMNode> sppmBacktrace(const Object *scene, const Ray &ray, int depth, int index, Vec3 prevColor = Vec3(1.), double prob = 1.)
{
    std::vector<SPPMNode> result;

    if (prevColor.len() < eps || prob < eps)
        return result;

    Hit hit;

    if (!scene->intersect(ray, hit))
        return result;

    int into = 0;
    Texture *texture = hit.texture();
    Vec3 inter = ray.pointAt(hit.t());
    auto feature = getFeature(*texture, inter);
    Vec3 color = feature.second;
    Vec3 normal = hit.normal(), nl = normal;
    if (normal.dot(ray.dir()) > 0)
        nl = -nl;
    else
        into = 1;

    double p = color.L_inf();
    if (p < eps)
        return result;

    if (depth > 4)
    {
        if (rand01() < p)
            color /= p;
        else
            return result;
    }

    Ray reflectRay = Ray(inter, ray.dir().reflect(nl));

    switch (feature.first)
    {
    case Texture::Reflect_t::DIFF:
        result.push_back(SPPMNode(inter, prevColor * color, nl, 1., index, prob));
        break;
    case Texture::Reflect_t::SPEC:
    {
        auto tmp = sppmBacktrace(scene, reflectRay, depth + 1, index, prevColor * color, prob);
        result.insert(result.end(), tmp.begin(), tmp.end());
        break;
    }
    case Texture::Reflect_t::REFR:
    {
        Vec3 refractDir = ray.dir().refract(normal, into ? 1. : texture->brdf(), into ? texture->brdf() : 1.);
        Ray refractRay = Ray(inter, refractDir);
        if (refractDir.squaredLen() < eps) //
            return sppmBacktrace(scene, reflectRay, depth + 1, index, prevColor * color, prob);

        double a = texture->brdf() - 1, b = texture->brdf() + 1;
        double R0 = sqr(a) / sqr(b);
        double c = 1. - (into ? -ray.dir().dot(nl) : refractDir.dot(normal));
        double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re;
        double P = 0.25 + 0.5 * Re, RP = Re / P, TP = Tr / (1 - P);

        if (depth > 1)
        {
            if (rand01() < P)
            {
                auto tmp = sppmBacktrace(scene, reflectRay, depth + 1, index, prevColor * color, prob * RP);
                result.insert(result.end(), tmp.begin(), tmp.end());
            }
            else
            {
                auto tmp = sppmBacktrace(scene, refractRay, depth + 1, index, prevColor * color, prob * TP);
                result.insert(result.end(), tmp.begin(), tmp.end());
            }
        }
        else
        {
            auto tmp = sppmBacktrace(scene, reflectRay, depth + 1, index, prevColor * color, prob * Re);
            result.insert(result.end(), tmp.begin(), tmp.end());
            auto tmp1 = sppmBacktrace(scene, refractRay, depth + 1, index, prevColor * color, prob * Tr);
            result.insert(result.end(), tmp1.begin(), tmp1.end());
        }
    }
    }
    return result;
}

void sppmForward(const Object *scene, const Ray &ray, int depth, const Vec3 &prevColor, IMGbuffer *img, KDtree *kdt, double prob = 1.)
{
    if (prevColor.L_inf() < eps)
        return;

    Hit hit;

    if (!scene->intersect(ray, hit))
        return;

    int into = 0;
    Texture *texture = hit.texture();
    Vec3 inter = ray.pointAt(hit.t());
    auto feature = getFeature(*texture, inter);
    Vec3 color = feature.second;
    Vec3 normal = hit.normal(), nl = normal;

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

    if (depth > 4)
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
        return sppmForward(scene, Ray(inter, dir), depth + 1, prevColor * color, img, kdt, prob);
    }
    else
    {
        Ray reflectRay = Ray(inter, ray.dir().reflect(nl));

        if (feature.first == Texture::Reflect_t::SPEC)
        {
            return sppmForward(scene, reflectRay, depth + 1, color * prevColor, img, kdt, prob);
        }
        else
        {
            Vec3 refractDir = ray.dir().refract(normal, into ? 1. : texture->brdf(), into ? texture->brdf() : 1.);
            Ray refractRay = Ray(inter, refractDir);
            if (refractDir.squaredLen() < eps) //
                return sppmForward(scene, reflectRay, depth + 1, prevColor * color, img, kdt, prob);

            double a = texture->brdf() - 1, b = texture->brdf() + 1;
            double R0 = sqr(a) / sqr(b);
            double c = 1. - (into ? -ray.dir().dot(nl) : refractDir.dot(normal));
            double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re;
            double P = 0.25 + 0.5 * Re, RP = Re / P, TP = Tr / (1 - P);

            if (depth > 1)
            {
                if (rand01() < P)
                    return sppmForward(scene, reflectRay, depth + 1, color * prevColor, img, kdt, prob * RP);
                else
                    return sppmForward(scene, refractRay, depth + 1, color * prevColor, img, kdt, prob * TP);
            }
            else
            {
                sppmForward(scene, reflectRay, depth + 1, color * prevColor, img, kdt, prob * Re);
                sppmForward(scene, refractRay, depth + 1, color * prevColor, img, kdt, prob * Tr);
                return;
            }
        }
    }
}
