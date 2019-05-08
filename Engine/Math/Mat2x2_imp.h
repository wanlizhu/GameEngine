#pragma once

#include "Mat2x2.h"
#include "Vec2.h"

namespace Engine
{
    template<typename T>
    Mat2x2<T>::Mat2x2()
    {
        Identity();
    }

    template<typename T>
    Mat2x2<T>::Mat2x2(const Mat2x2& mat) : mArray{mat[0], mat[1]}
    {
    }

    template<typename T>
    Mat2x2<T>::Mat2x2(Mat2x2&& mat) : mArray(std::move(mat.mArray))
    {
    }

    template<typename T>
    Mat2x2<T>::Mat2x2(const T& val) : mArray{
        row_type(val, 0),
        row_type(0, val)}
    {
    }

    template<typename T>
    Mat2x2<T>::Mat2x2(const T& val00, const T& val01,
                      const T& val10, const T& val11) : mArray{
        row_type(val00, val01),
        row_type(val10, val11)}
    {
    }

    template<typename T>
    Mat2x2<T>::Mat2x2(const row_type& vec0, const row_type& vec1) : mArray{vec0, vec1}
    {
    }

    template<typename T>
    typename Mat2x2<T>::size_type Mat2x2<T>::Size() const
    {
        return mArray.size();
    }

    template<typename T>
    void Mat2x2<T>::Identity()
    {
        mArray = {
            row_type(1, 0),
            row_type(0, 1)
        };
    }

    template<typename T>
    const typename Mat2x2<T>::row_type& Mat2x2<T>::operator[] (size_type index) const
    {
        return mArray[index];
    }

    template<typename T>
    typename Mat2x2<T>::row_type& Mat2x2<T>::operator[] (size_type index)
    {
        return const_cast<row_type&>(static_cast<const Mat2x2&>(*this)[index]);
    }

    template<typename T>
    Mat2x2<T>& Mat2x2<T>::operator= (const Mat2x2& mat)
    {
        mArray = mat.mArray;
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat2x2<T>& Mat2x2<T>::operator= (const Mat2x2<U>& mat)
    {
        mArray[0] = static_cast<row_type>(mat[0]);
        mArray[1] = static_cast<row_type>(mat[1]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat2x2<T>& Mat2x2<T>::operator+= (const U& scalar)
    {
        mArray[0] += static_cast<T>(scalar);
        mArray[1] += static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat2x2<T>& Mat2x2<T>::operator+= (const Mat2x2<U>& mat)
    {
        mArray[0] += static_cast<row_type>(mat[0]);
        mArray[1] += static_cast<row_type>(mat[1]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat2x2<T>& Mat2x2<T>::operator-= (const U& scalar)
    {
        mArray[0] -= static_cast<T>(scalar);
        mArray[1] -= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat2x2<T>& Mat2x2<T>::operator-= (const Mat2x2<U>& mat)
    {
        mArray[0] -= static_cast<row_type>(mat[0]);
        mArray[1] -= static_cast<row_type>(mat[1]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat2x2<T>& Mat2x2<T>::operator*= (const U& scalar)
    {
        mArray[0] *= static_cast<T>(scalar);
        mArray[1] *= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat2x2<T>& Mat2x2<T>::operator*= (const Mat2x2<U>& mat)
    {
        mArray[0] *= static_cast<row_type>(mat[0]);
        mArray[1] *= static_cast<row_type>(mat[1]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat2x2<T>& Mat2x2<T>::operator/= (const U& scalar)
    {
        mArray[0] /= static_cast<T>(scalar);
        mArray[1] /= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Mat2x2<T>& Mat2x2<T>::operator/= (const Mat2x2<U>& mat)
    {
        mArray[0] /= static_cast<row_type>(mat[0]);
        mArray[1] /= static_cast<row_type>(mat[1]);
        return *this;
    }

    template<typename T>
    Mat2x2<T> operator+ (const Mat2x2<T>& mat, const T& scalar)
    {
        return Mat2x2<T>(mat[0] + scalar, mat[1] + scalar);
    }

    template<typename T>
    Mat2x2<T> operator+ (const T& scalar, const Mat2x2<T>& mat)
    {
        return Mat2x2<T>(scalar + mat[0], scalar + mat[1]);
    }

    template<typename T>
    Mat2x2<T> operator+ (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return Mat2x2<T>(mat1[0] + mat2[0], mat1[1] + mat2[1]);
    }

    template<typename T>
    Mat2x2<T> operator- (const Mat2x2<T>& mat, const T& scalar)
    {
        return Mat2x2<T>(mat[0] - scalar, mat[1] - scalar);
    }

    template<typename T>
    Mat2x2<T> operator- (const T& scalar, const Mat2x2<T>& mat)
    {
        return Mat2x2<T>(scalar - mat[0], scalar - mat[1]);
    }

    template<typename T>
    Mat2x2<T> operator- (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return Mat2x2<T>(mat1[0] - mat2[0], mat1[1] - mat2[1]);
    }

    template<typename T>
    Mat2x2<T> operator* (const Mat2x2<T>& mat, const T& scalar)
    {
        return Mat2x2<T>(mat[0] * scalar, mat[1] * scalar);
    }

    template<typename T>
    Mat2x2<T> operator* (const T& scalar, const Mat2x2<T>& mat)
    {
        return Mat2x2<T>(scalar * mat[0], scalar * mat[1]);
    }

    template<typename T>
    Mat2x2<T> operator* (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return Mat2x2<T>(mat1[0] * mat2[0], mat1[1] * mat2[1]);
    }

    template<typename T>
    Mat2x2<T> operator/ (const Mat2x2<T>& mat, const T& scalar)
    {
        return Mat2x2<T>(mat[0] / scalar, mat[1] / scalar);
    }

    template<typename T>
    Mat2x2<T> operator/ (const T& scalar, const Mat2x2<T>& mat)
    {
        return Mat2x2<T>(scalar / mat[0], scalar / mat[1]);
    }

    template<typename T>
    Mat2x2<T> operator/ (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return Mat2x2<T>(mat1[0] / mat2[0], mat1[1] / mat2[1]);
    }

    template<typename T>
    bool operator== (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return mat1.mArray == mat2.mArray;
    }

    template<typename T>
    bool operator!= (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return mat1.mArray != mat2.mArray;
    }

    template<typename T>
    bool operator< (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return mat1.mArray < mat2.mArray;
    }

    template<typename T>
    bool operator<= (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return mat1.mArray <= mat2.mArray;
    }

    template<typename T>
    bool operator> (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return mat1.mArray > mat2.mArray;
    }

    template<typename T>
    bool operator>= (const Mat2x2<T>& mat1, const Mat2x2<T>& mat2)
    {
        return mat1.mArray >= mat2.mArray;
    }

    template<typename T>
    Mat2x2<T> operator- (const Mat2x2<T>& mat)
    {
        return Mat2x2<T>(-mat[0], -mat[1]);
    }

}