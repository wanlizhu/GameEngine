#pragma once

#ifdef __METAL_VERSION__
#include <metal_stdlib>
using namespace metal;
#else
#include "glm/glm.hpp"
using float2 = glm::vec2;
using float3 = glm::vec3;
using float4 = glm::vec4;
using float4x4 = glm::mat4;
#endif

// Vertex shader resources
#define VAO_POSITION_INDEX      0
#define VAO_NORMAL_INDEX        1
#define VAO_TEXCOORD_INDEX      2
#define MODEL_MATRIX_INDEX      3
#define VIEW_MATRIX_INDEX       4
#define PROJECTION_MATRIX_INDEX 5

// Fragment shader resources
#define CAMERA_POSITION_INDEX   6
#define LIGHT_POSITION_INDEX    7
#define MATERIAL_BLOCK_INDEX    8
#define BASE_COLOR_TEX_INDEX            0
#define METALLIC_ROUGHNESS_TEX_INDEX    1
#define NORMAL_TEX_INDEX                2
#define EMISSIVE_TEX_INDEX              3
#define OCCLUSION_TEX_INDEX             4

// Material flags
#define FLAG_USE_BASE_COLOR_TEX          1
#define FLAG_USE_METALLIC_ROUGHNESS_TEX (1 << 1)
#define FLAG_USE_NORMAL_TEX             (1 << 2)
#define FLAG_USE_EMISSIVE_TEX           (1 << 3)
#define FLAG_USE_OCCLUSION_TEX          (1 << 4)

// Material fields
#define MAT_FLAGS_NAME       "flags"
#define MAT_BASE_COLOR_NAME  "baseColor"
#define MAT_METALLIC_NAME    "metallic"
#define MAT_ROUGHNESS_NAME   "roughness"
#define MAT_EMISSIVE_NAME    "emissive"
#define MAT_AO_NAME          "ao"

struct MaterialUniforms
{
    int flags;
    float3 baseColor;
    float  metallic;
    float  roughness;
    float3 emissive;
    float  ao;
};
