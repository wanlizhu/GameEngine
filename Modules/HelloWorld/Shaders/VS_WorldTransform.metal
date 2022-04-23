#include <metal_stdlib>
#include <simd/simd.h>
#include "ShaderTypes.h"

using namespace metal;

vertex RasterData VS_WorldTransform(Vertex in [[stage_in]],
                                    constant Uniforms_Camera& camera [[buffer(UBO_Camera)]],
                                    constant Uniforms_Model& model [[buffer(UBO_Model)]])
{
    RasterData raster;
    raster.position = (model.matrixModel * camera.matrixViewProj) * float4(in.position, 1.0);
    raster.normal = in.normal;
    raster.texcoord = in.texcoord;
    
    return raster;
}
