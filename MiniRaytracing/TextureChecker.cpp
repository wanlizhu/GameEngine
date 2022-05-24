#include "TextureChecker.h"
#include "TextureSolidColor.h"

TextureChecker::TextureChecker(Texture* odd,
                               Texture* even)
    : _odd(odd->shared_from_this())
    , _even(even->shared_from_this())
{}

vec4 TextureChecker::sample(const vec2& uv, const vec3& pos) const
{
    if (_odd == nullptr || _even == nullptr)
        return vec4(0, 1, 1, 1); // solid cyan for debugging

    FLOAT sines = sin(10 * pos.x) * sin(10 * pos.y) * sin(10 * pos.z);
    return (sines < 0) ? _odd->sample(uv, pos) : _even->sample(uv, pos);
}

std::shared_ptr<Texture>
make_checker(const vec4& odd, const vec4& even)
{
    auto odd_tex  = make_solid_color(odd);
    auto even_tex = make_solid_color(even);

    return std::make_shared<TextureChecker>(odd_tex.get(), even_tex.get());
}

std::shared_ptr<Texture>
make_checker(Texture* odd, Texture* even)
{
    return std::make_shared<TextureChecker>(odd, even);
}