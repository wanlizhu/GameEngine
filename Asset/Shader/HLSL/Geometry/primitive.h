#ifndef _PRIM_COMMON_H__
#define _PRIM_COMMON_H__

#define PTEX_BITS (10) 
#define PTEX_WIDTH (0x1 << PTEX_BITS)
#define PTEX_MASK (PTEX_WIDTH - 1)

static const float LINE_WEIGHT_EXTEND = 1.15f;
static const float DISTANCE_TO_CENTER_EXTEND = 1.2f;

static const float GaussTheta = 10.0f;;
static const float GaussSigma = 52.5f;

struct NullVertex_Input
{
    int VertexID : TEXCOORD0;
    int InstanceID : TEXCOORD1;
};

float get_extended_line_weight(in float weight)
{
    return (weight * LINE_WEIGHT_EXTEND);
}

float get_extended_dist_to_center(in float dist)
{
    return (dist * DISTANCE_TO_CENTER_EXTEND);
}

int2 get_ptex_offset(uint offset)
{
    int2 tex_offset = int2(offset & PTEX_MASK, offset >> PTEX_BITS);
    return tex_offset;
}

bool in_center_of_wide_line(float dist, float weight)
{
    return ((weight > 1.5f) && (dist < weight / 2 - 0.5));
}

float gauss_blur(float dist, float weight, float theta, float sigma)
{
    float adjust_sigma = (0.02 * sigma);
    float sigma_2 = -adjust_sigma * adjust_sigma * 0.5f;

    float  adjust_theta = theta * 0.01f + 1.0f;

    float amt;
    amt = dist - (weight - 1.0f) * 0.5f;
    amt = max(0.0f, amt);

    return (adjust_theta * exp(amt * amt / sigma_2));
}

float adjust_single_line_alpha(float weight, float val)
{
    float ret = 0.0f;

    if (weight > 1.5f)
        ret = val;
    else
        ret = val * 0.8f;

    return saturate(ret);
}

float get_antialiasing_val(float dist, float weight)
{
    if ((dist == 0.0f) && (weight == 0.0f))
        return 0.0f;

    if (in_center_of_wide_line(dist, weight))
        return 1.0f;

    float val = gauss_blur(dist, weight, GaussTheta, GaussSigma);

    return adjust_single_line_alpha(weight, val);
}

float4 unpack_color(uint color)
{
    return float4(((color & 0x00ff0000) >> 16) / 255.0f,
        ((color & 0x0000ff00) >> 8) / 255.0f,
        ((color & 0x000000ff) >> 0) / 255.0f,
        ((color & 0xff000000) >> 24) / 255.0f);
}

float4 get_formatted_color(uint color, float alpha)
{
    float4 result = unpack_color(color);
    result.a *= alpha;
    return result;
}

float4 compute_final_color(float dist, float weight, uint color)
{
    float4 final_color;

    float anti_aliasing_val = get_antialiasing_val(dist, weight);
    final_color = get_formatted_color(color, anti_aliasing_val);

    return final_color;
}

#endif