#pragma once

#include "Texture.h"

class TextureSolidColor : public Texture
{
public:
    TextureSolidColor() = default;
    TextureSolidColor(const RGBA32& color);

    virtual RGBA32 sample(const vec2& uv, const vec3& pos) const override;

private:
    RGBA32 _color = RGBA32(0, 1, 1, 1); // solid cyan for debugging
};

std::shared_ptr<Texture> make_solid_color(const RGBA32& color);

