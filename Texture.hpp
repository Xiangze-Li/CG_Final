#pragma once

#include <string>
#include <utility>
#include "Vec3.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "deps/stb_image.h"
#endif

class Texture
{
public:
    enum class Reflect_t
    {
        DIFF, // DIFFuse
        SPEC, // SPECular
        REFR  // REFRactive
    };
    using Feature = std::pair<Reflect_t, Vec3>;

private:
    Vec3 _color;
    Reflect_t _refl;
    double _brdf;
    std::string _textureFile;
    unsigned char *_buff;
    int _width, _height, _channel;

public:
    Texture(Vec3 color = Vec3(1.), Reflect_t refl = Texture::Reflect_t::DIFF, double brdf = 1.5)
        : _textureFile(), _color(color), _refl(refl), _buff(nullptr),
          _brdf(brdf), _width(-1), _height(-1), _channel(-1) {}
    Texture(const std::string &filename)
        : _textureFile(filename), _color(0.), _refl(Reflect_t::DIFF), _brdf(1.5)
    {
        _buff = stbi_load(_textureFile.c_str(), &_width, &_height, &_channel, 0);
    }
    Texture(const Texture &r)
        : _textureFile(r._textureFile), _color(r._color), _refl(r._refl), _brdf(r._brdf)
    {
        if (_textureFile.empty())
        {
            _buff = nullptr;
            _width = _height = _channel = -1;
        }
        else
        {
            _buff = stbi_load(_textureFile.c_str(), &_width, &_height, &_channel, 0);
        }
    }
    ~Texture()
    {
        stbi_image_free(_buff);
    }

    Feature getFeature(double x, double y) const
    {
        if (_buff == nullptr)
            return std::make_pair(_refl, _color);

        int posX = (int(x * _width) % _width + _width) % _width;
        int posY = (int(y * _height) % _height + _height) % _height;
        int index = posY * _width * _channel + posX * _channel;
        int r = _buff[index + 0], g = _buff[index + 1], b = _buff[index + 2];

        return std::make_pair(Reflect_t::DIFF, (Vec3(r, g, b) / 255.));
    }

    std::string filename() const { return _textureFile; }
    double brdf() const { return _brdf; }
};

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
