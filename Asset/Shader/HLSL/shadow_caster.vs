#include "shadow_caster.h"

cbuffer TransformCB : register(b0)
{
    row_major float4x4 gWorldMatrix : WORLD;
    row_major float4x4 gLightViewMatrix : LIGHT_VIEW;
    row_major float4x4 gLightProjMatrix : LIGHT_PROJ;
};

ShadowCaster_VertexAttr ShadowCaster_VS(ShadowCaster_Input input)
{
    ShadowCaster_VertexAttr output = (ShadowCaster_VertexAttr)0;

    output.position.xyz = input.Position.xyz;
    output.position.w = 1.0f;

    output.position = mul(output.position, gWorldMatrix);
    output.position = mul(output.position, gLightViewMatrix);
    output.position = mul(output.position, gLightProjMatrix);

    return output;
}