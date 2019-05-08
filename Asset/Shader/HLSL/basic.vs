#include "basic.h"

float4x4 gWorldMatrix : WORLD;
float4x4 gViewMatrix : VIEW;
float4x4 gProjectionView : PROJECTION;

BasicPrimitive_VertexAttr BasicPrimitive_VS(BasicPrimitive_Input input)
{
    BasicPrimitive_VertexAttr output = (BasicPrimitive_VertexAttr)0;

    output.position.xyz = input.Position.xyz;
    output.position.w = 1.0f;

    output.position = mul(output.position, gWorldMatrix);
    output.position = mul(output.position, gViewMatrix);
    output.position = mul(output.position, gProjectionView);

    output.normal = input.Normal;

    return output;
}