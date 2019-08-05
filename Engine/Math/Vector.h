#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

#include "Utility.h"

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

    class Vec
    {
    public:
        template<typename T>
        static inline typename T::value_type Dot(const T& vec1, const T& vec2)
        {
            static_assert(std::is_base_of<Vec, T>::value, "T must inherit from Vec");
            T::value_type ret = 0;
            MATH_LOOP_OPERATION(i, T::DIMS, ret += vec1[i] * vec2[i]);
            return ret;
        }

        template<typename T>
        static inline typename T::value_type LengthSquared(const T& vec)
        {
            static_assert(std::is_base_of<Vec, T>::value, "T must inherit from Vec");
            return Dot(vec, vec);
        }

        template<typename T>
        static inline typename T::value_type Length(const T& vec)
        {
            static_assert(std::is_base_of<Vec, T>::value, "T must inherit from Vec");
            return sqrt(LengthSquared(vec));
        }

        template<typename T>
        static inline T Normalize(const T& vec)
        {
            static_assert(std::is_base_of<Vec, T>::value, "T must inherit from Vec");
            auto ret = vec;
            auto length = Length(vec);
            return ret / length;
        }

        template<typename T>
        static inline T Cross(const T& vec1, const T& vec2)
        {
            static_assert(std::is_base_of<Vec, T>::value, "T must inherit from Vec");
            static_assert(T::DIMS == 3, "T must have 3 dimension");
            T ret;
            ret.x = vec1.y * vec2.z - vec1.z * vec2.y;
            ret.y = vec1.z * vec2.x - vec1.x * vec2.z;
            ret.z = vec1.x * vec2.y - vec1.y * vec2.x;
            return ret;
        }

    protected:
        Vec() = default;
    };
}