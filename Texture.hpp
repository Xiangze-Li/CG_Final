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
    Vec3 _emission;
    Reflect_t _refl;
    double _brdf;
    std::string _textureFile;
    unsigned char *_buff;
    int _width, _height, _channel;

public:
    Texture(Vec3 color = Vec3(1.), Vec3 emission = Vec3(0.), Reflect_t refl = Texture::Reflect_t::DIFF, double brdf = 1.5, std::string textureFile = "")
        : _textureFile(textureFile), _color(color), _emission(emission), _refl(refl), _brdf(brdf)
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
    Texture(const Texture &r)
        : _textureFile(r._textureFile), _color(r._color), _emission(r._emission), _refl(r._refl), _brdf(r._brdf)
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

        return std::make_pair(_refl, (Vec3(r, g, b) / 255.));
    }

    std::string filename() const { return _textureFile; }
    double brdf() const { return _brdf; }
};
