#pragma once

#include "Mat2x2.h"
#include "Mat3x3.h"
#include "Mat4x4.h"

#include "Utility.h"

namespace Engine
{
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

    class Mat
    {
    public:
        template<typename T>
        static inline typename T Mul(const T& mat1, const T& mat2)
        {
            static_assert(std::is_base_of<Mat, T>::value, "T must inherit from Mat");
            T ret;
            MATH_LOOP_OPERATION(i, T::ROW, MATH_LOOP_OPERATION(j, T::COL, ret[i][j] = Vec::Dot(mat1.Row(i), mat2.Col(j))));
            return ret;
        }

    protected:
        Mat() = default;
    };
}