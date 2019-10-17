#include "PlaneMesh.h"

using namespace Engine;

PlaneMesh::PlaneMesh()
{
    float3 position[] =
    {
        //Plane
        float3(-10.f, 1.f, -10.f),
        float3(-10.f, 1.f,  10.f),
        float3( 10.f, 1.f, -10.f),
        float3( 10.f, 1.f,  10.f),
    };

    float3 normal[] = 
    {
        //Plane
        float3(0.f, 1.f, 0.f),
        float3(0.f, 1.f, 0.f),
        float3(0.f, 1.f, 0.f),
        float3(0.f, 1.f, 0.f),
    };

    short indices[] = 
    {
        0, 1, 3,
        0, 3, 2,
    };

    AttachVertexData<float3>(position, sizeof(position) / sizeof (float3), Attribute::ESemanticType::Position, "POSITION");
    AttachVertexData<float3>(normal, sizeof(position) / sizeof (float3), Attribute::ESemanticType::Normal, "NORMAL");
    AttachIndexData<short>(indices, sizeof(indices) / sizeof(short));
}

PlaneMesh::~PlaneMesh()
{
}