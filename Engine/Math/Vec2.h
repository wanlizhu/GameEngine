#pragma once

#include <array>
#include <Vector.h>

namespace Engine
{
    template<typename T>
    class Vec2 : public Vec
    {
    public:
        typedef Vec2<T> type;
        typedef T value_type;
        typedef size_t size_type;

        constexpr static int DIMS = 2;
        union
        {
            T mData[DIMS];
            std::array<T, DIMS> mArray;
            struct{ T x, y; };
        };

        Vec2();
        Vec2(const Vec2& vec);
        Vec2(Vec2&& vec);
        Vec2(const T& val);
        Vec2(const T (&val)[2]);
        Vec2(const T& val1, const T& val2);

        size_type Size() const;

        const T& operator[] (size_type index) const;
        T& operator[](size_type index);

        Vec2& operator= (const Vec2& vec);
        template<typename U>
        Vec2& operator= (const Vec2<U>& vec);

        template<typename U>
        Vec2& operator+= (const U& scalar);
        template<typename U>
        Vec2& operator+= (const Vec2<U>& vec);

        template<typename U>
        Vec2& operator-= (const U& scalar);
        template<typename U>
        Vec2& operator-= (const Vec2<U>& vec);

        template<typename U>
        Vec2& operator*= (const U& scalar);
        template<typename U>
        Vec2& operator*= (const Vec2<U>& vec);

        template<typename U>
        Vec2& operator/= (const U& scalar);
        template<typename U>
        Vec2& operator/= (const Vec2<U>& vec);
    };

    template<typename T>
    Vec2<T> operator+ (const Vec2<T>& vec, const T& scalar);
    template<typename T>
    Vec2<T> operator+ (const T& scalar, const Vec2<T>& vec);
    template<typename T>
    Vec2<T> operator+ (const Vec2<T>& vec1, const Vec2<T>& vec2);

    template<typename T>
    Vec2<T> operator- (const Vec2<T>& vec, const T& scalar);
    template<typename T>
    Vec2<T> operator- (const T& scalar, const Vec2<T>& vec);
    template<typename T>
    Vec2<T> operator- (const Vec2<T>& vec1, const Vec2<T>& vec2);

    template<typename T>
    Vec2<T> operator* (const Vec2<T>& vec, const T& scalar);
    template<typename T>
    Vec2<T> operator* (const T& scalar, const Vec2<T>& vec);
    template<typename T>
    Vec2<T> operator* (const Vec2<T>& vec1, const Vec2<T>& vec2);

    template<typename T>
    Vec2<T> operator/ (const Vec2<T>& vec, const T& scalar);
    template<typename T>
    Vec2<T> operator/ (const T& scalar, const Vec2<T>& vec);
    template<typename T>
    Vec2<T> operator/ (const Vec2<T>& vec1, const Vec2<T>& vec2);

    template<typename T>
    bool operator== (const Vec2<T>& vec1, const Vec2<T>& vec2);
    template<typename T>
    bool operator!= (const Vec2<T>& vec1, const Vec2<T>& vec2);

    template<typename T>
    bool operator< (const Vec2<T>& vec1, const Vec2<T>& vec2);
    template<typename T>
    bool operator<= (const Vec2<T>& vec1, const Vec2<T>& vec2);

    template<typename T>
    bool operator> (const Vec2<T>& vec1, const Vec2<T>& vec2);
    template<typename T>
    bool operator>= (const Vec2<T>& vec1, const Vec2<T>& vec2);

    template<typename T>
    Vec2<T> operator- (const Vec2<T>& vec);
}

#include "Vec2_imp.h"