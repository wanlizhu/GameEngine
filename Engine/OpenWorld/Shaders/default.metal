#include <metal_stdlib>
using namespace metal;

struct Vertex
{
    float3 position [[attribute(0)]];
    float3 normal   [[attribute(1)]];
    float2 texcoord [[attribute(2)]];
};

struct Raster
{
    float4 position [[position]];
    float3 normal;
    float2 texcoord;
};

vertex
Raster VertexShaderDefault(Vertex in [[stage_in]],
                           constant float4x4& viewMatrix [[buffer(3)]],
                           constant float4x4& projectionMatrix [[buffer(4)]],
                           constant float4x4& modelMatrix [[buffer(5)]])
{
    Raster raster;
    raster.position = projectionMatrix * viewMatrix * modelMatrix * float4(in.position, 1.0);
    raster.normal = in.normal;
    raster.texcoord = in.texcoord;
    
    return raster;
}

fragment
float4 FragmentShaderDefault(Raster in [[stage_in]])
{
    return float4(in.texcoord.xy, 0.0, 1.0);
}
