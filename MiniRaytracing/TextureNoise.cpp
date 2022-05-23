#include "TextureNoise.h"

PerlinNoise::PerlinNoise()
{
    _randoms = new vec3[_point_count];
    for (int i = 0; i < _point_count; ++i) 
        _randoms[i] = glm::normalize(vec3(random2()));

    _perm_x = perlin_generate_perm();
    _perm_y = perlin_generate_perm();
    _perm_z = perlin_generate_perm();
}

PerlinNoise::~PerlinNoise()
{
    delete[] _randoms;
    delete[] _perm_x;
    delete[] _perm_y;
    delete[] _perm_z;
}

FLOAT PerlinNoise::noise(const vec3& pos) const
{
    auto u = pos.x - floor(pos.x);
    auto v = pos.y - floor(pos.y);
    auto w = pos.z - floor(pos.z);
    auto i = static_cast<int>(floor(pos.x));
    auto j = static_cast<int>(floor(pos.y));
    auto k = static_cast<int>(floor(pos.z));
    vec3 c[2][2][2];

    for (int di = 0; di < 2; di++)
        for (int dj = 0; dj < 2; dj++)
            for (int dk = 0; dk < 2; dk++)
            {
                c[di][dj][dk] = _randoms[
                    _perm_x[(i + di) & 255] ^
                    _perm_y[(j + dj) & 255] ^
                    _perm_z[(k + dk) & 255]
                ];
            }

    return perlin_interp(c, u, v, w);
}

FLOAT PerlinNoise::turb(const vec3& pos, int depth) const
{
    FLOAT accum = 0.0;
    vec3  temp_p = pos;
    FLOAT weight = 1.0;

    for (int i = 0; i < depth; i++)
    {
        accum += weight * noise(temp_p);
        weight *= 0.5;
        temp_p *= 2;
    }

    return fabs(accum);
}

int* PerlinNoise::perlin_generate_perm()
{
    auto p = new int[_point_count];
    for (int i = 0; i < _point_count; i++)
        p[i] = i;

    permute(p, _point_count);
    return p;
}

void PerlinNoise::permute(int* p, int n)
{
    for (int i = n - 1; i > 0; i--) 
    {
        int target = random_in(0, i);
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
}

FLOAT PerlinNoise::perlin_interp(vec3 c[2][2][2], FLOAT u, FLOAT v, FLOAT w)
{
    auto uu = u * u * (3 - 2 * u);
    auto vv = v * v * (3 - 2 * v);
    auto ww = w * w * (3 - 2 * w);
    auto accum = 0.0;

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++) 
            {
                vec3 weight_v(u - i, v - j, w - k);
                accum += 
                    (i * uu + (1 - i) * (1 - uu)) *
                    (j * vv + (1 - j) * (1 - vv)) *
                    (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
            }

    return accum;
}


TextureNoise::TextureNoise(FLOAT scale)
    : _scale(scale)
{}

RGBA32 TextureNoise::sample(const vec2& uv, const vec3& pos) const
{
    vec3 color = vec3(1.0) * FLOAT(0.5) * FLOAT(1.0 + sin(_scale * pos.z + 10.0 * _noise.turb(pos)));
    return RGBA32(color.r, color.g, color.b, 1.0);
}

std::shared_ptr<Texture> make_noise(FLOAT scale)
{
    return std::make_shared<TextureNoise>(scale);
}