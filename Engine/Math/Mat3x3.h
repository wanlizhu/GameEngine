#pragma once

#include <array>

namespace Engine
{
    template <typename T>
    class Vec3;

    template<typename T>
    class Mat3x3
    {
    public:
        typedef Mat3x3<T> type;
        typedef Vec3<T> col_type;
        typedef Vec3<T> row_type;
        typedef T value_type;
        typedef size_t size_type;

        union
        {
            T mData[3][3];
            std::array<col_type, 3> mArray;
            struct
            {
                T x00, x01, x02,
                  x10, x11, x12,
                  x20, x21, x22;
            };
        };

        Mat3x3();
        Mat3x3(const Mat3x3& mat);
        Mat3x3(Mat3x3&& mat);
        Mat3x3(const T& val);
        Mat3x3(const T& val00, const T& val01, const T& val02,
               const T& val10, const T& val11, const T& val12,
               const T& val20, const T& val21, const T& val22);
        Mat3x3(const col_type& vec0, const col_type& vec1, const col_type& vec2);

        size_type Size() const;

        void Identity();

        const col_type& operator[] (size_type index) const;
        col_type& operator[](size_type index);

        Mat3x3& operator= (const Mat3x3& mat);
        template<typename U>
        Mat3x3& operator= (const Mat3x3<U>& mat);

        template<typename U>
        Mat3x3& operator+= (const U& scalar);
        template<typename U>
        Mat3x3& operator+= (const Mat3x3<U>& mat);

        template<typename U>
        Mat3x3& operator-= (const U& scalar);
        template<typename U>
        Mat3x3& operator-= (const Mat3x3<U>& mat);

        template<typename U>
        Mat3x3& operator*= (const U& scalar);
        template<typename U>
        Mat3x3& operator*= (const Mat3x3<U>& mat);

        template<typename U>
        Mat3x3& operator/= (const U& scalar);
        template<typename U>
        Mat3x3& operator/= (const Mat3x3<U>& mat);
    };

    template<typename T>
    Mat3x3<T> operator+ (const Mat3x3<T>& mat, const T& scalar);
    template<typename T>
    Mat3x3<T> operator+ (const T& scalar, const Mat3x3<T>& mat);
    template<typename T>
    Mat3x3<T> operator+ (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);

    template<typename T>
    Mat3x3<T> operator- (const Mat3x3<T>& mat, const T& scalar);
    template<typename T>
    Mat3x3<T> operator- (const T& scalar, const Mat3x3<T>& mat);
    template<typename T>
    Mat3x3<T> operator- (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);

    template<typename T>
    Mat3x3<T> operator* (const Mat3x3<T>& mat, const T& scalar);
    template<typename T>
    Mat3x3<T> operator* (const T& scalar, const Mat3x3<T>& mat);
    template<typename T>
    Mat3x3<T> operator* (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);

    template<typename T>
    Mat3x3<T> operator/ (const Mat3x3<T>& mat, const T& scalar);
    template<typename T>
    Mat3x3<T> operator/ (const T& scalar, const Mat3x3<T>& mat);
    template<typename T>
    Mat3x3<T> operator/ (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);

    template<typename T>
    bool operator== (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);
    template<typename T>
    bool operator!= (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);

    template<typename T>
    bool operator< (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);
    template<typename T>
    bool operator<= (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);

    template<typename T>
    bool operator> (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);
    template<typename T>
    bool operator>= (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2);

    template<typename T>
    Mat3x3<T> operator- (const Mat3x3<T>& mat);
}

#include "Mat3x3_imp.h"