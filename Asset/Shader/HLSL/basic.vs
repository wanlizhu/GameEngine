#include "basic.h"

float4x4 LookAt(float3 eye, float3 at, float3 up)
{
    float3 z = normalize(at - eye);
    float3 x = normalize(cross(up, z));
    float3 y = cross(z, x);

    float4x4 result = {
        x.x, y.x, z.x, 0.0f,
        x.y, y.y, z.y, 0.0f,
        x.z, y.z, z.z, 0.0f,
        -dot(x, eye), -dot(y, eye), -dot(z, eye), 1.0f
    };

    return result;
}

float4x4 Projection(float fovy, float aspect, float zn, float zf)
{
    float D2R = 3.14f / 180.0f;
    float yScale = 1.0f / tan(D2R * fovy / 2.f);
    float xScale = yScale / aspect;

    float4x4 result = {
        xScale, 0.0f, 0.0f, 0.0f,
        0.0f, yScale, 0.0f, 0.0f,
        0.0f, 0.0f, zf / (zf - zn), 1.0f,
        0.0f, 0.0f, -zn * zf / (zf - zn), 0.0f
    };

    return result;
}

BasicPrimitive_VertexAttr BasicPrimitive_VS(BasicPrimitive_Input input)
{
    BasicPrimitive_VertexAttr output = (BasicPrimitive_VertexAttr)0;

    output.position.xyz = input.Position.xyz;
    output.position.w = 1.0f;

    float4x4 viewMatrix = LookAt(float3(-2.5f, 1.0f, -2.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, -1.0f));
    float4x4 projectionMatrix = Projection(60.0f, 1080.0f / 1080.0f, 1.0f, 100.0f);

    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.normal = input.Normal;

    return output;
}