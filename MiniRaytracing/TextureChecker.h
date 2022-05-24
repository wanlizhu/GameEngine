#pragma once

#include "Texture.h"

class TextureChecker : public Texture
{
public:
    TextureChecker(Texture* odd, Texture* even);

    virtual vec4 sample(const vec2& uv, const vec3& pos) const override;

private:
    std::shared_ptr<Texture> _odd;
    std::shared_ptr<Texture> _even;
};

std::shared_ptr<Texture> make_checker(const vec4& odd, const vec4& even);
std::shared_ptr<Texture> make_checker(Texture* odd, Texture* even);
