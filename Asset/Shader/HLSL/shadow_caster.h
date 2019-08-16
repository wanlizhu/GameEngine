#ifndef _SHADOW_CASTER_H_
#define _SHADOW_CASTER_H_

struct ShadowCaster_Input
{
    float3 Position : POSITION;
};

struct ShadowCaster_VertexAttr
{
    float4 position : SV_Position;
};

#endif