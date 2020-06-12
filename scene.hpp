#pragma once

#include "Object.hpp"
#include "BezierCurve.hpp"

const double bezier_div_x = 3;
const double bezier_div_y = 2.5;
double control_x[] = {20. / bezier_div_x, 27. / bezier_div_x, 30. / bezier_div_x, 30. / bezier_div_x, 30. / bezier_div_x, 25. / bezier_div_x, 20. / bezier_div_x, 15. / bezier_div_x, 30. / bezier_div_x};
double control_y[] = {0. / bezier_div_y, 0. / bezier_div_y, 10. / bezier_div_y, 20. / bezier_div_y, 30. / bezier_div_y, 40. / bezier_div_y, 60. / bezier_div_y, 70. / bezier_div_y, 80. / bezier_div_y};
// BezierCurve bezier(control_x, control_y, 9, 9, .365);

Object *camera_left[] = {
    new Sphere(Texture("wallls.com_156455.png",
                       Vec3(.1, .25, .25), Vec3(0.), Texture::Reflect_t::DIFF, 1.5),
               Vec3(1e5 + 1, 40.8, 81.6), 1e5) //Left
};

Object **scene = camera_left;
int scene_num = 14;

Texture::Feature getFeature(Object *obj, const Texture &texture, Vec3 x)
{
    Texture::Feature feature;
    if (texture.filename().empty())
        feature = texture.getFeature(x.z(), x.x());
    else if (texture.filename() == "star.png")
        feature = texture.getFeature(x.z() / 15, x.x() / 15);
    else if (texture.filename() == "crack.jpg")
        feature = texture.getFeature(x.z() / 300, x.x() / 300);
    else if (texture.filename() == "wood.jpg")
        feature = texture.getFeature(x.x() / 30, x.z() / 30);
    else if (texture.filename() == "greenbg.jpg")
        feature = texture.getFeature(-x.x() / 125, -x.y() / 80 - 0.05);
    else if (texture.filename() == "wallls.com_156455.png")
        feature = texture.getFeature(-x.z() / 150, -x.y() / 100);
    else if (texture.filename() == "vase.png")
    {
        auto objj = dynamic_cast<BezierSurface *>(obj);
        Vec3 tmp = objj->change_for_bezier(x);
        feature = texture.getFeature(tmp.x() / 2 / PI + .5, tmp.y());
        if (rand01() < 0.2)
            feature.first = Texture::Reflect_t::SPEC;
    }
    else if (texture.filename() == "rainbow.png")
    {
        double px = (x.x() - 73) / 16.5, py = (x.y() - 16.5) / 16.5;
        feature = texture.getFeature((py * cos(-0.3) + px * sin(-0.3)) * .6 - .25, x.z());
    }
    else
        feature = texture.getFeature(x.z(), x.x());
    return feature;
}
