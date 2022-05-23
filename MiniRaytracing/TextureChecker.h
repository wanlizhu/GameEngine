#pragma once

#include "Texture.h"

class TextureChecker : public Texture
{
public:
    TextureChecker() = default;
    TextureChecker(std::shared_ptr<Texture> odd,
                   std::shared_ptr<Texture> even);

    virtual RGBA32 sample(const vec2& uv, const vec3& pos) const override;

private:
    std::shared_ptr<Texture> _odd;
    std::shared_ptr<Texture> _even;
};

std::shared_ptr<Texture> make_checker(const RGBA32& odd, const RGBA32& even);
