#include <metal_stdlib>
#include "ShaderConstants.h"

using namespace metal;

struct Vertex
{
    float3 position [[attribute(VAO_POSITION_INDEX)]];
    float3 normal   [[attribute(VAO_NORMAL_INDEX)]];
    float2 texcoord [[attribute(VAO_TEXCOORD_INDEX)]];
};

struct Raster
{
    float4 position [[position]]; // pixel space
    float3 positionWorld;         // world space
    float3 normal;
    float2 texcoord;
};

vertex
Raster VertexShaderDefault(Vertex in [[stage_in]],
                           constant float4x4& viewMatrix [[buffer(VIEW_MATRIX_INDEX)]],
                           constant float4x4& projectionMatrix [[buffer(PROJECTION_MATRIX_INDEX)]],
                           constant float4x4& modelMatrix [[buffer(MODEL_MATRIX_INDEX)]])
{
    Raster raster;
    raster.position = projectionMatrix * viewMatrix * modelMatrix * float4(in.position, 1.0);
    raster.positionWorld = (modelMatrix * float4(in.position, 1.0)).xyz;
    raster.normal = (modelMatrix * float4(in.normal, 0.0)).xyz;
    raster.texcoord = in.texcoord;
    
    return raster;
}

template<typename T>
inline T lerp(T weight, T begin, T end)
{
    return begin + (end - begin) * weight;
}

float distribution_GGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI_F * denom * denom;
    
    return nom / denom;
}

float geometry_schlick_GGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float geometry_smith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometry_schlick_GGX(NdotV, roughness);
    float ggx1  = geometry_schlick_GGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

inline float3 fresnel_schlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

fragment
float4 FragmentShaderDefault(Raster in [[stage_in]],
                             constant float3& lightPosition        [[buffer(LIGHT_POSITION_INDEX)]],
                             constant float3& cameraPosition       [[buffer(CAMERA_POSITION_INDEX)]],
                             constant MaterialUniforms& material   [[buffer(MATERIAL_BLOCK_INDEX)]],
                             texture2d<float> baseColorTex         [[texture(BASE_COLOR_TEX_INDEX)]],
                             texture2d<float> metallicRoughnessTex [[texture(METALLIC_ROUGHNESS_TEX_INDEX)]],
                             texture2d<float> normalTex    [[texture(NORMAL_TEX_INDEX)]],
                             texture2d<float> emissiveTex  [[texture(EMISSIVE_TEX_INDEX)]],
                             texture2d<float> occlusionTex [[texture(OCCLUSION_TEX_INDEX)]])
{
    constexpr sampler lsp(mip_filter::linear, mag_filter::linear, min_filter::linear);
    float3 color = float3(0.0, 0.0, 0.0);
    float3 cameraDirection = cameraPosition - in.positionWorld;
    float3 lightDirection = lightPosition - in.positionWorld;
    
    float3 albedo    = (material.flags & FLAG_USE_BASE_COLOR_TEX) ? baseColorTex.sample(lsp, in.texcoord).xyz : material.baseColor.xyz;
    float2 MR        = (material.flags & FLAG_USE_METALLIC_ROUGHNESS_TEX) ? metallicRoughnessTex.sample(lsp, in.texcoord).xy : float2(material.metallic, material.roughness);
    float3 normal    = (material.flags & FLAG_USE_NORMAL_TEX) ? normalTex.sample(lsp, in.texcoord).xyz : in.normal;
    float3 emissive  = (material.flags & FLAG_USE_EMISSIVE_TEX) ? emissiveTex.sample(lsp, in.texcoord).xyz : material.emissive.xyz;
    float  ao        = (material.flags & FLAG_USE_OCCLUSION_TEX) ? occlusionTex.sample(lsp, in.texcoord).x : 1.0;
    //float3 sss       = screenspaceShadowTex.sample(lsp, in.texcoord).xyz;;
    
    float  metallic  = MR.x;
    float  roughness = MR.y;
    
    float3 N = normalize(normal); // in.normal
    float3 V = normalize(-cameraDirection);
    
    float3 F0 = 0.04;
    F0 = lerp<float3>(F0, albedo, metallic);
    
    float3 Lo = 0.0;
    
    float3 L = normalize(-lightDirection);
    float3 H = normalize(V + L);
    
    float3 NDF = distribution_GGX(N, H, roughness);
    float  G = geometry_smith(N, V, L, roughness);
    float3 F = fresnel_schlick(max(dot(H, V), 0.0), F0);
    
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    float3 nominator = NDF * G * F;
    float  denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    float3 specular = nominator / denominator;
    
    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / M_PI_F + specular) * float3(1.0, 1.0, 1.0) * NdotL;
    
    float3 ambient = 0.03 * albedo * ao;
    color = ambient; //+ Lo * sss;
    
    color = color / (color + 1.0);
    color = pow(color, 1.0 / 2.2);
    
    color += emissive;
    
    return float4(color, 1.0);
}
