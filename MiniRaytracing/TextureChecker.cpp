#include "TextureChecker.h"
#include "TextureSolidColor.h"

TextureChecker::TextureChecker(std::shared_ptr<Texture> odd,
                               std::shared_ptr<Texture> even)
    : _odd(odd)
    , _even(even)
{}

RGBA32 TextureChecker::sample(const vec2& uv, const vec3& pos) const
{
    if (_odd == nullptr || _even == nullptr)
        return RGBA32(0, 1, 1, 1); // solid cyan for debugging

    FLOAT sines = sin(10 * pos.x) * sin(10 * pos.y) * sin(10 * pos.z);
    return (sines < 0) ? _odd->sample(uv, pos) : _even->sample(uv, pos);
}

std::shared_ptr<Texture> make_checker(const RGBA32& odd, const RGBA32& even)
{
    return std::make_shared<TextureChecker>(make_solid_color(odd),
                                            make_solid_color(even));
}