#include "rect.h"

Rect_VertexAttr Rect_VS(uint vertexID  : SV_VertexID)
{
    Rect_VertexAttr result = (Rect_VertexAttr)0;

    result.uv = float2((vertexID << 1) & 2, vertexID & 2);
    result.position = float4(result.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

    return result;
}