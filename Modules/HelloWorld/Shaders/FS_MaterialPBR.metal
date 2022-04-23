#include <metal_stdlib>
#include <simd/simd.h>
#include "ShaderTypes.h"

using namespace metal;

fragment float4 FS_MaterialPBR(RasterData in [[stage_in]],
                               constant Uniforms_MaterialPBR& uniforms [[buffer(UBO_MaterialPBR)]])
{
    return float4(1, 0, 0, 1);
}
