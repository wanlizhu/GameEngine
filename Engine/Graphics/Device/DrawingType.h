#pragma once

#include <stdint.h>

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat2x2.h"
#include "Mat3x3.h"
#include "Mat4x4.h"
#include "Box2.h"

namespace Engine
{
    typedef Vec2<bool> bool2;
    typedef Vec3<bool> bool3;
    typedef Vec4<bool> bool4;

    typedef Vec2<uint32_t> uint2;
    typedef Vec3<uint32_t> uint3;
    typedef Vec4<uint32_t> uint4;

    typedef Vec2<int32_t> int2;
    typedef Vec3<int32_t> int3;
    typedef Vec4<int32_t> int4;

    typedef Vec2<float> float2;
    typedef Vec3<float> float3;
    typedef Vec4<float> float4;

    typedef Vec2<double> double2;
    typedef Vec3<double> double3;
    typedef Vec4<double> double4;

    typedef Mat2x2<uint32_t> uint2x2;
    typedef Mat3x3<uint32_t> uint3x3;
    typedef Mat4x4<uint32_t> uint4x4;

    typedef Mat2x2<int32_t> int2x2;
    typedef Mat3x3<int32_t> int3x3;
    typedef Mat4x4<int32_t> int4x4;

    typedef Mat2x2<float> float2x2;
    typedef Mat3x3<float> float3x3;
    typedef Mat4x4<float> float4x4;

    typedef Mat2x2<double> double2x2;
    typedef Mat3x3<double> double3x3;
    typedef Mat4x4<double> double4x4;
}