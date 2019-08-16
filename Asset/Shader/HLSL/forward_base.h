#ifndef _FORWARD_BASE_H_
#define _FORWARD_BASE_H_

struct ForwardBase_Input
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct ForwardBase_VertexAttr
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 lightViewPosition : Position;
};

#endif