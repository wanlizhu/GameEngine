#pragma once

#include "Mat3x3.h"
#include "Vec3.h"

namespace Engine
{
    template<typename T>
    Mat3x3<T>::Mat3x3()
    {
        Identity();
    }

    template<typename T>
    Mat3x3<T>::Mat3x3(const Mat3x3& mat) : mArray{mat[0], mat[1], mat[2]}
    {
    }

    template<typename T>
    Mat3x3<T>::Mat3x3(Mat3x3&& mat) : mArray(std::move(mat.mArray))
    {
    }

    template<typename T>
    Mat3x3<T>::Mat3x3(const T& val) : mArray{
        col_type(val, 0, 0),
        col_type(0, val, 0),
        col_type(0, 0, val)}
    {
    }

    template<typename T>
    Mat3x3<T>::Mat3x3(const T& val00, const T& val01, const T& val02,
                      const T& val10, const T& val11, const T& val12,
                      const T& val20, const T& val21, const T& val22) : mArray{
        col_type(val00, val01, val02),
        col_type(val10, val11, val12),
        col_type(val20, val21, val22)}
    {
    }

    template<typename T>
    Mat3x3<T>::Mat3x3(const col_type& vec0, const col_type& vec1, const col_type& vec2) : mArray{vec0, vec1, vec2}
    {
    }

    template<typename T>
    typename Mat3x3<T>::size_type Mat3x3<T>::Size() const
    {
        return mArray.size();
    }

    template<typename T>
    void Mat3x3<T>::Identity()
    {
        mArray = {
            col_type(1, 0, 0),
            col_type(0, 1, 0),
            col_type(0, 0, 1)
        };
    }

    template<typename T>
    const typename Mat3x3<T>::col_type& Mat3x3<T>::operator[] (size_type index) const
    {
        return mArray[index];
    }

    template<typename T>
    typename Mat3x3<T>::col_type& Mat3x3<T>::operator[] (size_type index)
    {
        return const_cast<col_type&>(static_cast<const Mat3x3&>(*this)[index]);
    }

    template<typename T>
    Mat3x3<T>& Mat3x3<T>::operator= (const Mat3x3& mat)
    {
        mArray = mat.mArray;
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat3x3<T>& Mat3x3<T>::operator= (const Mat3x3<U>& mat)
    {
        mArray[0] = static_cast<col_type>(mat[0]);
        mArray[1] = static_cast<col_type>(mat[1]);
        mArray[2] = static_cast<col_type>(mat[2]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat3x3<T>& Mat3x3<T>::operator+= (const U& scalar)
    {
        mArray[0] += static_cast<T>(scalar);
        mArray[1] += static_cast<T>(scalar);
        mArray[2] += static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat3x3<T>& Mat3x3<T>::operator+= (const Mat3x3<U>& mat)
    {
        mArray[0] += static_cast<col_type>(mat[0]);
        mArray[1] += static_cast<col_type>(mat[1]);
        mArray[2] += static_cast<col_type>(mat[2]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat3x3<T>& Mat3x3<T>::operator-= (const U& scalar)
    {
        mArray[0] -= static_cast<T>(scalar);
        mArray[1] -= static_cast<T>(scalar);
        mArray[2] -= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat3x3<T>& Mat3x3<T>::operator-= (const Mat3x3<U>& mat)
    {
        mArray[0] -= static_cast<col_type>(mat[0]);
        mArray[1] -= static_cast<col_type>(mat[1]);
        mArray[2] -= static_cast<col_type>(mat[2]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat3x3<T>& Mat3x3<T>::operator*= (const U& scalar)
    {
        mArray[0] *= static_cast<T>(scalar);
        mArray[1] *= static_cast<T>(scalar);
        mArray[2] *= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat3x3<T>& Mat3x3<T>::operator*= (const Mat3x3<U>& mat)
    {
        mArray[0] *= static_cast<col_type>(mat[0]);
        mArray[1] *= static_cast<col_type>(mat[1]);
        mArray[2] *= static_cast<col_type>(mat[2]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat3x3<T>& Mat3x3<T>::operator/= (const U& scalar)
    {
        mArray[0] /= static_cast<T>(scalar);
        mArray[1] /= static_cast<T>(scalar);
        mArray[2] /= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat3x3<T>& Mat3x3<T>::operator/= (const Mat3x3<U>& mat)
    {
        mArray[0] /= static_cast<col_type>(mat[0]);
        mArray[1] /= static_cast<col_type>(mat[1]);
        mArray[2] /= static_cast<col_type>(mat[2]);
        return *this;
    }

    template<typename T>
    Mat3x3<T> operator+ (const Mat3x3<T>& mat, const T& scalar)
    {
        return Mat3x3<T>(mat[0] + scalar, mat[1] + scalar, mat[2] + scalar);
    }

    template<typename T>
    Mat3x3<T> operator+ (const T& scalar, const Mat3x3<T>& mat)
    {
        return Mat3x3<T>(scalar + mat[0], scalar + mat[1], scalar + mat[2]);
    }

    template<typename T>
    Mat3x3<T> operator+ (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return Mat3x3<T>(mat1[0] + mat2[0], mat1[1] + mat2[1], mat1[2] + mat2[2]);
    }

    template<typename T>
    Mat3x3<T> operator- (const Mat3x3<T>& mat, const T& scalar)
    {
        return Mat3x3<T>(mat[0] - scalar, mat[1] - scalar, mat[2] - scalar);
    }

    template<typename T>
    Mat3x3<T> operator- (const T& scalar, const Mat3x3<T>& mat)
    {
        return Mat3x3<T>(scalar - mat[0], scalar - mat[1], scalar - mat[2]);
    }

    template<typename T>
    Mat3x3<T> operator- (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return Mat3x3<T>(mat1[0] - mat2[0], mat1[1] - mat2[1], mat1[2] - mat2[2]);
    }

    template<typename T>
    Mat3x3<T> operator* (const Mat3x3<T>& mat, const T& scalar)
    {
        return Mat3x3<T>(mat[0] * scalar, mat[1] * scalar, mat[2] * scalar);
    }

    template<typename T>
    Mat3x3<T> operator* (const T& scalar, const Mat3x3<T>& mat)
    {
        return Mat3x3<T>(scalar * mat[0], scalar * mat[1], scalar * mat[2]);
    }

    template<typename T>
    Mat3x3<T> operator* (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return Mat3x3<T>(mat1[0] * mat2[0], mat1[1] * mat2[1], mat1[2] * mat2[2]);
    }

    template<typename T>
    Mat3x3<T> operator/ (const Mat3x3<T>& mat, const T& scalar)
    {
        return Mat3x3<T>(mat[0] / scalar, mat[1] / scalar, mat[2] / scalar);
    }

    template<typename T>
    Mat3x3<T> operator/ (const T& scalar, const Mat3x3<T>& mat)
    {
        return Mat3x3<T>(scalar / mat[0], scalar / mat[1], scalar / mat[2]);
    }

    template<typename T>
    Mat3x3<T> operator/ (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return Mat3x3<T>(mat1[0] / mat2[0], mat1[1] / mat2[1], mat1[2] / mat2[2]);
    }

    template<typename T>
    bool operator== (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return mat1.mArray == mat2.mArray;
    }

    template<typename T>
    bool operator!= (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return mat1.mArray != mat2.mArray;
    }

    template<typename T>
    bool operator< (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return mat1.mArray < mat2.mArray;
    }

    template<typename T>
    bool operator<= (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return mat1.mArray <= mat2.mArray;
    }

    template<typename T>
    bool operator> (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return mat1.mArray > mat2.mArray;
    }

    template<typename T>
    bool operator>= (const Mat3x3<T>& mat1, const Mat3x3<T>& mat2)
    {
        return mat1.mArray >= mat2.mArray;
    }

    template<typename T>
    Mat3x3<T> operator- (const Mat3x3<T>& mat)
    {
        return Mat3x3<T>(-mat[0], -mat[1], -mat[2]);
    }

}