#ifndef _BASIC_H_
#define _BASIC_H_

struct BasicPrimitive_Input
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct BasicPrimitive_VertexAttr
{
    noperspective float4 position : SV_Position;
    noperspective float3 normal : NORMAL;
};

#endif