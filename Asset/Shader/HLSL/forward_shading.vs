#include "forward_shading.h"

cbuffer TransformCB : register(b0)
{
    row_major float4x4 gWorldMatrix : WORLD;
    row_major float4x4 gViewMatrix : VIEW;
    row_major float4x4 gProjectionView : PROJECTION;
};

ForwardShading_VertexAttr ForwardShading_VS(ForwardShading_Input input)
{
    ForwardShading_VertexAttr output = (ForwardShading_VertexAttr)0;

    output.position.xyz = input.Position.xyz;
    output.position.w = 1.0f;
    output.position = mul(output.position, gWorldMatrix);
    output.position = mul(output.position, gViewMatrix);
    output.position = mul(output.position, gProjectionView);

    output.pos = output.position;
    output.normal = normalize(mul(input.Normal, (float3x3)gWorldMatrix));

    return output;
}