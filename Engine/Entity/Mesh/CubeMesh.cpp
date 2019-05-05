#include "CubeMesh.h"

using namespace Engine;

CubeMesh::CubeMesh()
{
    Vec3<float> position[28] =
    {
        // Front
        Vec3<float>(-1.f, -1.f, -1.f),
        Vec3<float>(-1.f,  1.f, -1.f),
        Vec3<float>( 1.f, -1.f, -1.f),
        Vec3<float>( 1.f,  1.f, -1.f),
        // Back
        Vec3<float>(-1.f, -1.f,  1.f),
        Vec3<float>(-1.f,  1.f,  1.f),
        Vec3<float>( 1.f, -1.f,  1.f),
        Vec3<float>( 1.f,  1.f,  1.f),
        // Right
        Vec3<float>( 1.f, -1.f, -1.f),
        Vec3<float>( 1.f,  1.f, -1.f),
        Vec3<float>( 1.f, -1.f,  1.f),
        Vec3<float>( 1.f,  1.f,  1.f),
        // Left
        Vec3<float>(-1.f, -1.f, -1.f),
        Vec3<float>(-1.f,  1.f, -1.f),
        Vec3<float>(-1.f, -1.f,  1.f),
        Vec3<float>(-1.f,  1.f,  1.f),
        // Up
        Vec3<float>(-1.f,  1.f, -1.f),
        Vec3<float>(-1.f,  1.f,  1.f),
        Vec3<float>( 1.f,  1.f, -1.f),
        Vec3<float>( 1.f,  1.f,  1.f),
        // Down
        Vec3<float>(-1.f, -1.f, -1.f),
        Vec3<float>(-1.f, -1.f,  1.f),
        Vec3<float>( 1.f, -1.f, -1.f),
        Vec3<float>( 1.f, -1.f,  1.f),

        //Plane
        Vec3<float>(-5.f, -1.f, -5.f),
        Vec3<float>(-5.f, -1.f,  5.f),
        Vec3<float>( 5.f, -1.f, -5.f),
        Vec3<float>( 5.f, -1.f,  5.f),
    };

    Vec3<float> normal[28] = 
    {
        // Front
        Vec3<float>(0.f, 0.f, -1.f),
        Vec3<float>(0.f, 0.f, -1.f),
        Vec3<float>(0.f, 0.f, -1.f),
        Vec3<float>(0.f, 0.f, -1.f),
        // Back
        Vec3<float>(0.f, 0.f,  1.f),
        Vec3<float>(0.f, 0.f,  1.f),
        Vec3<float>(0.f, 0.f,  1.f),
        Vec3<float>(0.f, 0.f,  1.f),
        // Right
        Vec3<float>(1.f, 0.f,  0.f),
        Vec3<float>(1.f, 0.f,  0.f),
        Vec3<float>(1.f, 0.f,  0.f),
        Vec3<float>(1.f, 0.f,  0.f),
        // Left
        Vec3<float>(-1.f, 0.f, 0.f),
        Vec3<float>(-1.f, 0.f, 0.f),
        Vec3<float>(-1.f, 0.f, 0.f),
        Vec3<float>(-1.f, 0.f, 0.f),
        // Up
        Vec3<float>(0.f,  1.f, 0.f),
        Vec3<float>(0.f,  1.f, 0.f),
        Vec3<float>(0.f,  1.f, 0.f),
        Vec3<float>(0.f,  1.f, 0.f),
        // Down
        Vec3<float>(0.f, -1.f, 0.f),
        Vec3<float>(0.f, -1.f, 0.f),
        Vec3<float>(0.f, -1.f, 0.f),
        Vec3<float>(0.f, -1.f, 0.f),

        //Plane
        Vec3<float>(0.f, 1.f, 0.f),
        Vec3<float>(0.f, 1.f, 0.f),
        Vec3<float>(0.f, 1.f, 0.f),
        Vec3<float>(0.f, 1.f, 0.f),
    };

    short indices[42] = 
    {
        27, 24, 25,
        27, 26, 24,

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

    AttachVertexData<Vec3<float>>(position, 28, Attribute::ESemanticType::Position, Attribute::EFormatType::Float3, "POSITION");
    AttachVertexData<Vec3<float>>(normal, 28, Attribute::ESemanticType::Normal, Attribute::EFormatType::Float3, "NORMAL");
    AttachIndexData<short>(indices, 42);
}

CubeMesh::~CubeMesh()
{
}