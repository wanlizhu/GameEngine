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

        template<typename T, typename U>
        static inline typename T Mul(const T& vec, const U& mat)
        {
            static_assert(std::is_base_of<Vec, T>::value, "T must inherit from Vec");
            static_assert(std::is_base_of<Mat, U>::value, "T must inherit from Mat");
            T ret;
            MATH_LOOP_OPERATION(i, T::DIMS, ret[i] = Vec::Dot(vec, mat.Col(i)));
            return ret;
        }

        template<typename T>
        static inline typename Mat3x3<T> RotateLH(T p, T h, T b)
        {
            T sinp, cosp, sinh, cosh, sinb, cosb;
            MATH_TYPE_DEGREE_FUN(T, p, sin, sinp)
            MATH_TYPE_DEGREE_FUN(T, p, cos, cosp)
            MATH_TYPE_DEGREE_FUN(T, h, sin, sinh)
            MATH_TYPE_DEGREE_FUN(T, h, cos, cosh)
            MATH_TYPE_DEGREE_FUN(T, b, sin, sinb)
            MATH_TYPE_DEGREE_FUN(T, b, cos, cosb)

            return Mat3x3<T>( cosb*cosh+sinb*sinp*sinh,     sinb*cosp,      -cosb*sinh+sinb*sinp*cosh,
                             -sinb*cosh+cosb*sinp*sinh,     cosb*cosp,       sinb*sinh+cosb*sinp*cosh,
                              cosp*sinh,                   -sinp,            cosp*cosh                 );
        }

        template<typename T>
        static inline typename Mat4x4<T> LookAtLH(const Vec3<T>& eye, const Vec3<T>& at, const Vec3<T>& up)
        {
            Vec3<T> z = Vec::Normalize(at - eye);
            Vec3<T> x = Vec::Normalize(Vec::Cross(up, z));
            Vec3<T> y = Vec::Cross(z, x);

            Mat4x4<T> view = {
                x.x, y.x, z.x, 0,
                x.y, y.y, z.y, 0,
                x.z, y.z, z.z, 0,
                Vec::Dot(-x, eye), Vec::Dot(-y, eye), Vec::Dot(-z, eye), 1
            };
            return view;
        }

        template<typename T>
        static inline typename Mat4x4<T> PerspectiveFovLH(T fovy, T aspect, T zn, T zf)
        {
            T f;
            MATH_TYPE_DEGREE_FUN(T, static_cast<T>(fovy / 2.0), tan, f);
            T yScale = static_cast<T>(1.0 / f);
            T xScale = yScale / aspect;

            Mat4x4<T> proj = {
                xScale, 0, 0, 0,
                0, yScale, 0, 0,
                0, 0, zf / (zf - zn), 1,
                0, 0, -zn * zf / (zf - zn), 0
            };

            return proj;
        }

        template<typename T>
        static inline typename Mat4x4<T> OrthoLH(T w, T h, T zn, T zf)
        {
            Mat4x4<T> ortho = {
                2 / w, 0, 0, 0,
                0, 2 / h, 0, 0,
                0, 0, 1 / (zf - zn), 0,
                0, 0, zn / (zn - zf), 1
            };

            return ortho;
        }

    protected:
        Mat() = default;
    };
}