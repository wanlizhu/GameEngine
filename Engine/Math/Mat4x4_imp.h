#pragma once

#include "Mat4x4.h"
#include "Vec4.h"

namespace Engine
{
    template<typename T>
    Mat4x4<T>::Mat4x4()
    {
        Identity();
    }

    template<typename T>
    Mat4x4<T>::Mat4x4(const Mat4x4& mat) : mArray{mat[0], mat[1], mat[2], mat[3]}
    {
    }

    template<typename T>
    Mat4x4<T>::Mat4x4(Mat4x4&& mat) : mArray(std::move(mat.mArray))
    {
    }

    template<typename T>
    Mat4x4<T>::Mat4x4(const T& val) : mArray{
        row_type(val, 0, 0, 0),
        row_type(0, val, 0, 0),
        row_type(0, 0, val, 0),
        row_type(0, 0, 0, val)}
    {
    }

    template<typename T>
    Mat4x4<T>::Mat4x4(const T& val00, const T& val01, const T& val02, const T& val03,
                      const T& val10, const T& val11, const T& val12, const T& val13,
                      const T& val20, const T& val21, const T& val22, const T& val23,
                      const T& val30, const T& val31, const T& val32, const T& val33) : mArray{
        row_type(val00, val01, val02, val03),
        row_type(val10, val11, val12, val13),
        row_type(val20, val21, val22, val23),
        row_type(val30, val31, val32, val33)}
    {
    }

    template<typename T>
    Mat4x4<T>::Mat4x4(const row_type& vec0, const row_type& vec1, const row_type& vec2, const row_type& vec3) : mArray{vec0, vec1, vec2, vec3}
    {
    }

    template<typename T>
    typename Mat4x4<T>::size_type Mat4x4<T>::Size() const
    {
        return mArray.size();
    }

    template<typename T>
    void Mat4x4<T>::Identity()
    {
        mArray = {
            row_type(1, 0, 0, 0),
            row_type(0, 1, 0, 0),
            row_type(0, 0, 1, 0),
            row_type(0, 0, 0, 1)
        };
    }

    template<typename T>
    const typename Mat4x4<T>::row_type& Mat4x4<T>::operator[] (size_type index) const
    {
        return mArray[index];
    }

    template<typename T>
    typename Mat4x4<T>::row_type& Mat4x4<T>::operator[] (size_type index)
    {
        return const_cast<row_type&>(static_cast<const Mat4x4&>(*this)[index]);
    }

    template<typename T>
    typename Mat4x4<T>::row_type Mat4x4<T>::Row(size_type index) const
    {
        return row_type(mData[index][0], mData[index][1], mData[index][2], mData[index][3]);
    }

    template<typename T>
    typename Mat4x4<T>::col_type Mat4x4<T>::Col(size_type index) const
    {
        return col_type(mData[0][index], mData[1][index], mData[2][index], mData[3][index]);
    }

    template<typename T>
    Mat4x4<T>& Mat4x4<T>::operator= (const Mat4x4& mat)
    {
        mArray = mat.mArray;
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat4x4<T>& Mat4x4<T>::operator= (const Mat4x4<U>& mat)
    {
        mArray[0] = static_cast<row_type>(mat[0]);
        mArray[1] = static_cast<row_type>(mat[1]);
        mArray[2] = static_cast<row_type>(mat[2]);
        mArray[3] = static_cast<row_type>(mat[3]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat4x4<T>& Mat4x4<T>::operator+= (const U& scalar)
    {
        mArray[0] += static_cast<T>(scalar);
        mArray[1] += static_cast<T>(scalar);
        mArray[2] += static_cast<T>(scalar);
        mArray[2] += static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat4x4<T>& Mat4x4<T>::operator+= (const Mat4x4<U>& mat)
    {
        mArray[0] += static_cast<row_type>(mat[0]);
        mArray[1] += static_cast<row_type>(mat[1]);
        mArray[2] += static_cast<row_type>(mat[2]);
        mArray[3] += static_cast<row_type>(mat[3]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat4x4<T>& Mat4x4<T>::operator-= (const U& scalar)
    {
        mArray[0] -= static_cast<T>(scalar);
        mArray[1] -= static_cast<T>(scalar);
        mArray[2] -= static_cast<T>(scalar);
        mArray[3] -= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat4x4<T>& Mat4x4<T>::operator-= (const Mat4x4<U>& mat)
    {
        mArray[0] -= static_cast<row_type>(mat[0]);
        mArray[1] -= static_cast<row_type>(mat[1]);
        mArray[2] -= static_cast<row_type>(mat[2]);
        mArray[3] -= static_cast<row_type>(mat[3]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat4x4<T>& Mat4x4<T>::operator*= (const U& scalar)
    {
        mArray[0] *= static_cast<T>(scalar);
        mArray[1] *= static_cast<T>(scalar);
        mArray[2] *= static_cast<T>(scalar);
        mArray[3] *= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat4x4<T>& Mat4x4<T>::operator*= (const Mat4x4<U>& mat)
    {
        mArray[0] *= static_cast<row_type>(mat[0]);
        mArray[1] *= static_cast<row_type>(mat[1]);
        mArray[2] *= static_cast<row_type>(mat[2]);
        mArray[3] *= static_cast<row_type>(mat[3]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat4x4<T>& Mat4x4<T>::operator/= (const U& scalar)
    {
        mArray[0] /= static_cast<T>(scalar);
        mArray[1] /= static_cast<T>(scalar);
        mArray[2] /= static_cast<T>(scalar);
        mArray[3] /= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat4x4<T>& Mat4x4<T>::operator/= (const Mat4x4<U>& mat)
    {
        mArray[0] /= static_cast<row_type>(mat[0]);
        mArray[1] /= static_cast<row_type>(mat[1]);
        mArray[2] /= static_cast<row_type>(mat[2]);
        mArray[3] /= static_cast<row_type>(mat[3]);
        return *this;
    }

    template<typename T>
    Mat4x4<T> operator+ (const Mat4x4<T>& mat, const T& scalar)
    {
        return Mat4x4<T>(mat[0] + scalar, mat[1] + scalar, mat[2] + scalar, mat[3] + scalar);
    }

    template<typename T>
    Mat4x4<T> operator+ (const T& scalar, const Mat4x4<T>& mat)
    {
        return Mat4x4<T>(scalar + mat[0], scalar + mat[1], scalar + mat[2], scalar + mat[3]);
    }

    template<typename T>
    Mat4x4<T> operator+ (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return Mat4x4<T>(mat1[0] + mat2[0], mat1[1] + mat2[1], mat1[2] + mat2[2], mat1[3] + mat2[3]);
    }

    template<typename T>
    Mat4x4<T> operator- (const Mat4x4<T>& mat, const T& scalar)
    {
        return Mat4x4<T>(mat[0] - scalar, mat[1] - scalar, mat[2] - scalar, mat[3] - scalar);
    }

    template<typename T>
    Mat4x4<T> operator- (const T& scalar, const Mat4x4<T>& mat)
    {
        return Mat4x4<T>(scalar - mat[0], scalar - mat[1], scalar - mat[2], scalar - mat[3]);
    }

    template<typename T>
    Mat4x4<T> operator- (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return Mat4x4<T>(mat1[0] - mat2[0], mat1[1] - mat2[1], mat1[2] - mat2[2], mat1[3] - mat2[3]);
    }

    template<typename T>
    Mat4x4<T> operator* (const Mat4x4<T>& mat, const T& scalar)
    {
        return Mat4x4<T>(mat[0] * scalar, mat[1] * scalar, mat[2] * scalar, mat[3] * scalar);
    }

    template<typename T>
    Mat4x4<T> operator* (const T& scalar, const Mat4x4<T>& mat)
    {
        return Mat4x4<T>(scalar * mat[0], scalar * mat[1], scalar * mat[2], scalar * mat[3]);
    }

    template<typename T>
    Mat4x4<T> operator* (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return Mat4x4<T>(mat1[0] * mat2[0], mat1[1] * mat2[1], mat1[2] * mat2[2], mat1[3] * mat2[3]);
    }

    template<typename T>
    Mat4x4<T> operator/ (const Mat4x4<T>& mat, const T& scalar)
    {
        return Mat4x4<T>(mat[0] / scalar, mat[1] / scalar, mat[2] / scalar, mat[3] / scalar);
    }

    template<typename T>
    Mat4x4<T> operator/ (const T& scalar, const Mat4x4<T>& mat)
    {
        return Mat4x4<T>(scalar / mat[0], scalar / mat[1], scalar / mat[2], scalar / mat[3]);
    }

    template<typename T>
    Mat4x4<T> operator/ (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return Mat4x4<T>(mat1[0] / mat2[0], mat1[1] / mat2[1], mat1[2] / mat2[2], mat1[3] / mat2[3]);
    }

    template<typename T>
    bool operator== (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return mat1.mArray == mat2.mArray;
    }

    template<typename T>
    bool operator!= (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return mat1.mArray != mat2.mArray;
    }

    template<typename T>
    bool operator< (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return mat1.mArray < mat2.mArray;
    }

    template<typename T>
    bool operator<= (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return mat1.mArray <= mat2.mArray;
    }

    template<typename T>
    bool operator> (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return mat1.mArray > mat2.mArray;
    }

    template<typename T>
    bool operator>= (const Mat4x4<T>& mat1, const Mat4x4<T>& mat2)
    {
        return mat1.mArray >= mat2.mArray;
    }

    template<typename T>
    Mat4x4<T> operator- (const Mat4x4<T>& mat)
    {
        return Mat4x4<T>(-mat[0], -mat[1], -mat[2], -mat[3]);
    }

}