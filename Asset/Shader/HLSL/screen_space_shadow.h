#ifndef _SCREEN_SPACE_SHADOW_H_
#define _SCREEN_SPACE_SHADOW_H_

struct ScreenSpaceShadow_Input
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct ScreenSpaceShadow_VertexAttr
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 lightViewPosition : Position;
};

#endif