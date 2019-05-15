#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "primitive.h"

static const float SINGLE_LINE_WIDTH = 1.0f;
static const float SINGLE_LINE_WIDTH_EXPAND = 10.2f;
static const float LINE_WEIGHT_EXPAND = 2.0f;
static const float ENDPOINT_EXTEND = 1.0f / 3.0f;

cbuffer ScreenCB : register(b0)
{
    float2 gInvScreenSize;
}

Texture2D<float3> gLineSegmentTex : LineSegmentTexture;

struct VertexAttr_Segment
{
    noperspective float4 position : SV_Position;
    nointerpolation uint color : COLOR;
    linear float dist : DIST;
};

struct SegmentAttr
{
    float2 startPoint;
    float2 endPoint;
    uint color;
    float drawZ;
    uint flag;
};

float2 get_unit_pos(uint vid)
{
    uint a = vid & 0x1;
    uint b = (vid & 0x2) >> 1;

    return float2(a, b);
}

float2 get_rect_pos(uint vid)
{
    float2 unit_pos = get_unit_pos(vid);

    return float2(unit_pos.x*2.0f - 1.0f, unit_pos.y*2.0f - 1.0f);
}

float2 screen_to_ndc_pos(float2 scr_pos)
{
    return scr_pos * gInvScreenSize * 2.0f - 1.0f;
}

float get_line_weight_expand(float weight)
{
    return weight + LINE_WEIGHT_EXPAND;
}

float2 get_line_envelope_pos(int vid, uint line_width, float2 start_point, float2 end_point, out float dist)
{
    float weight_expand = get_line_weight_expand(line_width);
    float2 uv = get_rect_pos(vid);

    float2 dir = normalize(end_point - start_point);

    float2 expand_start_point = start_point - dir * ENDPOINT_EXTEND;
    float2 expand_end_point = end_point + dir * ENDPOINT_EXTEND;

    float wide_line_expand = weight_expand * 0.5f + 1.0f;

    float2 scr_pos = (1 - uv.x) * expand_start_point * 0.5f + (1 + uv.x) * expand_end_point * 0.5f + uv.y * wide_line_expand * float2(dir.y, -dir.x);
    scr_pos += float2(0.5f, 0.5f);

    dist = uv.y * wide_line_expand;

    return screen_to_ndc_pos(scr_pos);
}

void load_single_segment_info(uint instanceID, out SegmentAttr attr)
{
    uint segmentID = instanceID;
    uint drawOrderZIndex = 0;

    uint line_index = segmentID * 2;
    int2 index_offset = get_ptex_offset(line_index);

    float3 val = gLineSegmentTex.Load(int3(index_offset, 0));

    attr.startPoint = val.xy;
    attr.endPoint.x = val.z;

    index_offset = get_ptex_offset(line_index + 1);
    val = gLineSegmentTex.Load(int3(index_offset, 0));

    attr.endPoint.y = val.x;
    attr.color = asuint(val.y);
    attr.drawZ = abs(val.z);
    attr.flag = 0;
}

#endif