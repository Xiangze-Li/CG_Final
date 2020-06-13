#pragma once

#include "Object.hpp"

Texture::Feature getFeature(const Texture &texture, Vec3 x)
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
    else if (texture.filename() == "rainbow.png")
    {
        double px = (x.x() - 73) / 16.5, py = (x.y() - 16.5) / 16.5;
        feature = texture.getFeature((py * cos(-0.3) + px * sin(-0.3)) * .6 - .25, x.z());
    }
    else
        feature = texture.getFeature(x.z(), x.x());
    return feature;
}
