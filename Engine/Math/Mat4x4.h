#pragma once

#include <array>
#include <Matrix.h>

namespace Engine
{
    template <typename T>
    class Vec4;

    template<typename T>
    class Mat4x4 : public Mat
    {
    public:
        typedef Mat4x4<T> type;
        typedef Vec4<T> col_type;
        typedef Vec4<T> row_type;
        typedef T value_type;
        typedef size_t size_type;

        constexpr static int COL = 4;
        constexpr static int ROW = 4;
        union
        {
            T mData[4][4];
            std::array<row_type, 4> mArray;
            struct
            {
                T x00, x01, x02, x03,
                  x10, x11, x12, x13,
                  x20, x21, x22, x23,
                  x30, x31, x32, x33;
            };
        };

        Mat4x4();
        Mat4x4(const Mat4x4& mat);
        Mat4x4(Mat4x4&& mat);
        Mat4x4(const T& val);
        Mat4x4(const T& val00, const T& val01, const T& val02, const T& val03,
               const T& val10, const T& val11, const T& val12, const T& val13,
               const T& val20, const T& val21, const T& val22, const T& val23,
               const T& val30, const T& val31, const T& val32, const T& val33);
        Mat4x4(const row_type& vec0, const row_type& vec1, const row_type& vec2, const row_type& vec3);

        size_type Size() const;

        void Identity();

        const row_type& operator[] (size_type index) const;
        row_type& operator[](size_type index);

        row_type Row(size_type index) const;
        col_type Col(size_type index) const;

        Mat4x4& operator= (const Mat4x4& mat);
        template<typename U>
        Mat4x4& operator= (const Mat4x4<U>& mat);

        template<typename U>
        Mat4x4& operator+= (const U& scalar);
        template<typename U>
        Mat4x4& operator+= (const Mat4x4<U>& mat);

        template<typename U>
        Mat4x4& operator-= (const U& scalar);
        template<typename U>
        Mat4x4& operator-= (const Mat4x4<U>& mat);

        template<typename U>
        Mat4x4& operator*= (const U& scalar);
        template<typename U>
        Mat4x4& operator*= (const Mat4x4<U>& mat);

        template<typename U>
        Mat4x4& operator/= (const U& scalar);
        template<typename U>
        Mat4x4& operator/= (const Mat4x4<U>& mat);
    };

    template<typename T>
    Mat4x4<T> operator+ (const Mat4x4<T>& mat, const T& scalar);
    template<typename T>
    Mat4x4<T> operator+ (const T& scalar, const Mat4x4<T>& mat);
    template<typename T>
    Mat4x4<T> operator+ (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);

    template<typename T>
    Mat4x4<T> operator- (const Mat4x4<T>& mat, const T& scalar);
    template<typename T>
    Mat4x4<T> operator- (const T& scalar, const Mat4x4<T>& mat);
    template<typename T>
    Mat4x4<T> operator- (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);

    template<typename T>
    Mat4x4<T> operator* (const Mat4x4<T>& mat, const T& scalar);
    template<typename T>
    Mat4x4<T> operator* (const T& scalar, const Mat4x4<T>& mat);
    template<typename T>
    Mat4x4<T> operator* (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);

    template<typename T>
    Mat4x4<T> operator/ (const Mat4x4<T>& mat, const T& scalar);
    template<typename T>
    Mat4x4<T> operator/ (const T& scalar, const Mat4x4<T>& mat);
    template<typename T>
    Mat4x4<T> operator/ (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);

    template<typename T>
    bool operator== (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);
    template<typename T>
    bool operator!= (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);

    template<typename T>
    bool operator< (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);
    template<typename T>
    bool operator<= (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);

    template<typename T>
    bool operator> (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);
    template<typename T>
    bool operator>= (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2);

    template<typename T>
    Mat4x4<T> operator- (const Mat4x4<T>& mat);
}

#include "Mat4x4_imp.h"