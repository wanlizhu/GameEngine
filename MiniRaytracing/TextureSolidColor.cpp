#include "TextureSolidColor.h"

TextureSolidColor::TextureSolidColor(const vec4& color)
    : _color(color)
{}

vec4 TextureSolidColor::sample(const vec2& uv, const vec3& pos) const
{
    return _color;
}

std::shared_ptr<Texture> 
make_solid_color(const vec4& color)
{
    return std::make_shared<TextureSolidColor>(color);
}