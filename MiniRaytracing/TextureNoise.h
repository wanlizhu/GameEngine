#pragma once

#include "Texture.h"

class PerlinNoise
{
public:
    PerlinNoise();
    virtual ~PerlinNoise();

    FLOAT noise(const vec3& pos) const;
    FLOAT turb(const vec3& pos, int depth = 7) const;

private:
    static int* perlin_generate_perm();
    static void permute(int* p, int n);
    static FLOAT perlin_interp(vec3 c[2][2][2], FLOAT u, FLOAT v, FLOAT w);

private:
    static const int _point_count = 256;
    vec3* _randoms;
    int* _perm_x;
    int* _perm_y;
    int* _perm_z;
};

class TextureNoise : public Texture
{
public:
    TextureNoise(int style, FLOAT scale);

    virtual vec4 sample(const vec2& uv, const vec3& pos) const override;

private:
    PerlinNoise _noise;
    FLOAT _scale = 1.0;
    int _style = 1;
};

std::shared_ptr<Texture> make_noise(int style, FLOAT scale);

