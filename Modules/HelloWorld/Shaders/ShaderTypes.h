#pragma once

#include "../Config.h"

#ifdef __METAL_VERSION__
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct Vertex
{
    float3 position [[attribute(VBO_Position)]];
    float3 normal   [[attribute(VBO_Normal)]];
    float3 tangent  [[attribute(VBO_Tangent)]];
    float2 texcoord [[attribute(VBO_Texcoord)]];
};

struct RasterData
{
    float4 position [[position]];
    float3 normal;
    float2 texcoord;
};

#else
using float3 = glm::vec3;
using float4 = glm::vec4;
using float3x3 = glm::mat3;
using float4x4 = glm::mat4;
#endif

struct Uniforms_Camera
{
    float4x4 matrixViewProj;
};

struct Uniforms_Model
{
    float4x4 matrixModel;
};

struct Uniforms_Light
{
    
};

struct Uniforms_MaterialPBR
{
    float4 albedo;
    float metallic;
    float roughness;
};
