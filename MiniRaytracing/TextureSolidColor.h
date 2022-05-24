#pragma once

#include "Texture.h"

class TextureSolidColor : public Texture
{
public:
    TextureSolidColor(const vec4& color);

    virtual vec4 sample(const vec2& uv, const vec3& pos) const override;

private:
    vec4 _color = vec4(0, 1, 1, 1); // solid cyan for debugging
};

std::shared_ptr<Texture> make_solid_color(const vec4& color);

