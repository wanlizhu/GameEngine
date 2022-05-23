#pragma once

#include "Texture.h"

class TextureImage : public Texture
{
public:
    TextureImage() = default;
    TextureImage(const std::string& path);

    virtual RGBA32 sample(const vec2 & uv, const vec3 & pos) const override;

private:
    std::shared_ptr<std::vector<RGBA>> _data;
    int _width = 0;
    int _height = 0;
};

std::shared_ptr<Texture> make_image(const std::string& path);

