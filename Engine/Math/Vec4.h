#pragma once

#include <array>
#include <Vector.h>

namespace Engine
{
    template<typename T>
    class Vec4 : public Vec
    {
    public:
        typedef Vec4<T> type;
        typedef T value_type;
        typedef size_t size_type;

        constexpr static int DIMS = 4;
        union
        {
            T mData[DIMS];
            std::array<T, DIMS> mArray;
            struct{ T x, y, z, w; };
        };

        Vec4();
        Vec4(const Vec4& vec);
        Vec4(Vec4&& vec);
        Vec4(const T& val);
        Vec4(const T& val1, const T& val2, const T& val3, const T& val4);

        size_type Size() const;

        const T& operator[] (size_type index) const;
        T& operator[](size_type index);

        Vec4& operator= (const Vec4& vec);
        template<typename U>
        Vec4& operator= (const Vec4<U>& vec);

        template<typename U>
        Vec4& operator+= (const U& scalar);
        template<typename U>
        Vec4& operator+= (const Vec4<U>& vec);

        template<typename U>
        Vec4& operator-= (const U& scalar);
        template<typename U>
        Vec4& operator-= (const Vec4<U>& vec);

        template<typename U>
        Vec4& operator*= (const U& scalar);
        template<typename U>
        Vec4& operator*= (const Vec4<U>& vec);

        template<typename U>
        Vec4& operator/= (const U& scalar);
        template<typename U>
        Vec4& operator/= (const Vec4<U>& vec);
    };

    template<typename T>
    Vec4<T> operator+ (const Vec4<T>& vec, const T& scalar);
    template<typename T>
    Vec4<T> operator+ (const T& scalar, const Vec4<T>& vec);
    template<typename T>
    Vec4<T> operator+ (const Vec4<T>& vec1, const Vec4<T>& vec2);

    template<typename T>
    Vec4<T> operator- (const Vec4<T>& vec, const T& scalar);
    template<typename T>
    Vec4<T> operator- (const T& scalar, const Vec4<T>& vec);
    template<typename T>
    Vec4<T> operator- (const Vec4<T>& vec1, const Vec4<T>& vec2);

    template<typename T>
    Vec4<T> operator* (const Vec4<T>& vec, const T& scalar);
    template<typename T>
    Vec4<T> operator* (const T& scalar, const Vec4<T>& vec);
    template<typename T>
    Vec4<T> operator* (const Vec4<T>& vec1, const Vec4<T>& vec2);

    template<typename T>
    Vec4<T> operator/ (const Vec4<T>& vec, const T& scalar);
    template<typename T>
    Vec4<T> operator/ (const T& scalar, const Vec4<T>& vec);
    template<typename T>
    Vec4<T> operator/ (const Vec4<T>& vec1, const Vec4<T>& vec2);

    template<typename T>
    bool operator== (const Vec4<T>& vec1, const Vec4<T>& vec2);
    template<typename T>
    bool operator!= (const Vec4<T>& vec1, const Vec4<T>& vec2);

    template<typename T>
    bool operator< (const Vec4<T>& vec1, const Vec4<T>& vec2);
    template<typename T>
    bool operator<= (const Vec4<T>& vec1, const Vec4<T>& vec2);

    template<typename T>
    bool operator> (const Vec4<T>& vec1, const Vec4<T>& vec2);
    template<typename T>
    bool operator>= (const Vec4<T>& vec1, const Vec4<T>& vec2);

    template<typename T>
    Vec4<T> operator- (const Vec4<T>& vec);
}

#include "Vec4_imp.h"