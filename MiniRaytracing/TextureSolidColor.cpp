#include "TextureSolidColor.h"

TextureSolidColor::TextureSolidColor(const RGBA32& color)
    : _color(color)
{}

RGBA32 TextureSolidColor::sample(const vec2& uv, const vec3& pos) const 
{
    return _color;
}

std::shared_ptr<Texture> make_solid_color(const RGBA32& color)
{
    return std::make_shared<TextureSolidColor>(color);
}