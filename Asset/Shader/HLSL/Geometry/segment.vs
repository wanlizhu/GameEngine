#include "segment.h"

void set_single_segment_propertties(uint vid, SegmentAttr seg_attr, float width, out VertexAttr_Segment output)
{
    float temp_dist = 0.0f;
    bool temp_isDot = false;

    output.position.xy = get_line_envelope_pos(vid, width, seg_attr.startPoint, seg_attr.endPoint, temp_dist);

    output.position.z = seg_attr.drawZ;
    output.position.w = 1.0f;

    output.dist = temp_dist;
    output.color = seg_attr.color;
}


VertexAttr_Segment Segment_VS(NullVertex_Input input)
{
    SegmentAttr seg_attr = (SegmentAttr)0;

    load_single_segment_info(input.InstanceID, seg_attr);

    float width = SINGLE_LINE_WIDTH_EXPAND;
    VertexAttr_Segment output = (VertexAttr_Segment)0;
    set_single_segment_propertties(input.VertexID, seg_attr, width, output);

    return output;
}