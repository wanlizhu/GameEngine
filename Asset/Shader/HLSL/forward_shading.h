#ifndef _FORWARD_SHADING_H_
#define _FORWARD_SHADING_H_

struct ForwardShading_Input
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct ForwardShading_VertexAttr
{
    float4 position : SV_Position;
    float4 pos : POSITION;
    float3 normal : NORMAL;
};

#endif