#include "CubeMesh.h"

using namespace Engine;

CubeMesh::CubeMesh()
{
    float3 position[] =
    {
        // Front
        float3(-0.5f, -0.5f, -0.5f),
        float3(-0.5f,  0.5f, -0.5f),
        float3( 0.5f, -0.5f, -0.5f),
        float3( 0.5f,  0.5f, -0.5f),
        // Back
        float3(-0.5f, -0.5f,  0.5f),
        float3(-0.5f,  0.5f,  0.5f),
        float3( 0.5f, -0.5f,  0.5f),
        float3( 0.5f,  0.5f,  0.5f),
        // Right
        float3( 0.5f, -0.5f, -0.5f),
        float3( 0.5f,  0.5f, -0.5f),
        float3( 0.5f, -0.5f,  0.5f),
        float3( 0.5f,  0.5f,  0.5f),
        // Left
        float3(-0.5f, -0.5f, -0.5f),
        float3(-0.5f,  0.5f, -0.5f),
        float3(-0.5f, -0.5f,  0.5f),
        float3(-0.5f,  0.5f,  0.5f),
        // Up
        float3(-0.5f,  0.5f, -0.5f),
        float3(-0.5f,  0.5f,  0.5f),
        float3( 0.5f,  0.5f, -0.5f),
        float3( 0.5f,  0.5f,  0.5f),
        // Down
        float3(-0.5f, -0.5f, -0.5f),
        float3(-0.5f, -0.5f,  0.5f),
        float3( 0.5f, -0.5f, -0.5f),
        float3( 0.5f, -0.5f,  0.5f)
    };

    float3 normal[] = 
    {
        // Front
        float3(0.f, 0.f, -1.f),
        float3(0.f, 0.f, -1.f),
        float3(0.f, 0.f, -1.f),
        float3(0.f, 0.f, -1.f),
        // Back
        float3(0.f, 0.f,  1.f),
        float3(0.f, 0.f,  1.f),
        float3(0.f, 0.f,  1.f),
        float3(0.f, 0.f,  1.f),
        // Right
        float3(1.f, 0.f,  0.f),
        float3(1.f, 0.f,  0.f),
        float3(1.f, 0.f,  0.f),
        float3(1.f, 0.f,  0.f),
        // Left
        float3(-1.f, 0.f, 0.f),
        float3(-1.f, 0.f, 0.f),
        float3(-1.f, 0.f, 0.f),
        float3(-1.f, 0.f, 0.f),
        // Up
        float3(0.f,  1.f, 0.f),
        float3(0.f,  1.f, 0.f),
        float3(0.f,  1.f, 0.f),
        float3(0.f,  1.f, 0.f),
        // Down
        float3(0.f, -1.f, 0.f),
        float3(0.f, -1.f, 0.f),
        float3(0.f, -1.f, 0.f),
        float3(0.f, -1.f, 0.f)
    };

    short indices[] = 
    {
        0, 1, 3,
        0, 3, 2,
        6, 7, 5,
        6, 5, 4,

        10, 8, 9,
        10, 9, 11,
        12, 14, 15,
        12, 15, 13,

        18, 16, 17,
        18, 17, 19,
        23, 21, 20,
        23, 20, 22,
    };

    AttachVertexData<float3>(position, sizeof(position) / sizeof (float3), Attribute::ESemanticType::Position, Attribute::EFormatType::Float3, "POSITION");
    AttachVertexData<float3>(normal, sizeof(position) / sizeof (float3), Attribute::ESemanticType::Normal, Attribute::EFormatType::Float3, "NORMAL");
    AttachIndexData<short>(indices, sizeof(indices) / sizeof(short));
}

CubeMesh::~CubeMesh()
{
}