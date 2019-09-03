#include "screen_space_shadow.h"

cbuffer TransformCB : register(b0)
{
    row_major float4x4 gWorldMatrix : WORLD;
    row_major float4x4 gViewMatrix : VIEW;
    row_major float4x4 gProjectionView : PROJECTION;
};

cbuffer TransformLight : register(b1)
{
    row_major float4x4 gLightViewMatrix : LIGHT_VIEW;
    row_major float4x4 gLightProjMatrix : LIGHT_PROJ;
};

ScreenSpaceShadow_VertexAttr ScreenSpaceShadow_VS(ScreenSpaceShadow_Input input)
{
    ScreenSpaceShadow_VertexAttr output = (ScreenSpaceShadow_VertexAttr)0;

    output.position.xyz = input.Position.xyz;
    output.position.w = 1.0f;
    output.position = mul(output.position, gWorldMatrix);
    output.position = mul(output.position, gViewMatrix);
    output.position = mul(output.position, gProjectionView);

    output.normal = normalize(mul(input.Normal, (float3x3)gWorldMatrix));

    output.lightViewPosition.xyz = input.Position.xyz;
    output.lightViewPosition.w = 1.0f;
    output.lightViewPosition = mul(output.lightViewPosition, gWorldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, gLightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, gLightProjMatrix);

    return output;
}