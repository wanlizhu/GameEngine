#pragma once

#include <math.h>

namespace Engine
{
    template<typename T>
    Vec3<T>::Vec3() : mArray{T(), T(), T()}
    {
    }

    template<typename T>
    Vec3<T>::Vec3(const Vec3& vec) : mArray{vec[0], vec[1], vec[2]}
    {
    }

    template<typename T>
    Vec3<T>::Vec3(Vec3&& vec) : mArray(std::move(vec.mArray))
    {
    }

    template<typename T>
    Vec3<T>::Vec3(const T& val) : mArray{val, val, val}
    {
    }

    template<typename T>
    Vec3<T>::Vec3(const T (&val)[3]) : mArray{val[0], val[1], val[2]}
    {
    }

    template<typename T>
    Vec3<T>::Vec3(const T& val1, const T& val2, const T& val3) : mArray{val1, val2, val3}
    {
    }

    template<typename T>
    typename Vec3<T>::size_type Vec3<T>::Size() const
    {
        return mArray.size();
    }

    template<typename T>
    const T& Vec3<T>::operator[] (size_type index) const
    {
        return mArray[index];
    }

    template<typename T>
    T& Vec3<T>::operator[](size_type index)
    {
        return const_cast<T&>(static_cast<const Vec3&>(*this)[index]);
    }

    template<typename T>
    Vec3<T>& Vec3<T>::operator= (const Vec3& vec)
    {
        mArray = vec.mArray;
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec3<T>& Vec3<T>::operator= (const Vec3<U>& vec)
    {
        mArray[0] = static_cast<T>(vec[0]);
        mArray[1] = static_cast<T>(vec[1]);
        mArray[2] = static_cast<T>(vec[2]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec3<T>& Vec3<T>::operator+= (const U& scalar)
    {
        mArray[0] += static_cast<T>(scalar);
        mArray[1] += static_cast<T>(scalar);
        mArray[2] += static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec3<T>& Vec3<T>::operator+= (const Vec3<U>& vec)
    {
        mArray[0] += static_cast<T>(vec[0]);
        mArray[1] += static_cast<T>(vec[1]);
        mArray[2] += static_cast<T>(vec[2]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec3<T>& Vec3<T>::operator-= (const U& scalar)
    {
        mArray[0] -= static_cast<T>(scalar);
        mArray[1] -= static_cast<T>(scalar);
        mArray[2] -= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec3<T>& Vec3<T>::operator-= (const Vec3<U>& vec)
    {
        mArray[0] -= static_cast<T>(vec[0]);
        mArray[1] -= static_cast<T>(vec[1]);
        mArray[2] -= static_cast<T>(vec[2]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec3<T>& Vec3<T>::operator*= (const U& scalar)
    {
        mArray[0] *= static_cast<T>(scalar);
        mArray[1] *= static_cast<T>(scalar);
        mArray[2] *= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec3<T>& Vec3<T>::operator*= (const Vec3<U>& vec)
    {
        mArray[0] *= static_cast<T>(vec[0]);
        mArray[1] *= static_cast<T>(vec[1]);
        mArray[2] *= static_cast<T>(vec[2]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec3<T>& Vec3<T>::operator/= (const U& scalar)
    {
        mArray[0] /= static_cast<T>(scalar);
        mArray[1] /= static_cast<T>(scalar);
        mArray[2] /= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec3<T>& Vec3<T>::operator/= (const Vec3<U>& vec)
    {
        mArray[0] /= static_cast<T>(vec[0]);
        mArray[1] /= static_cast<T>(vec[1]);
        mArray[2] /= static_cast<T>(vec[2]);
        return *this;
    }

    template<typename T>
    Vec3<T> operator+ (const Vec3<T>& vec, const T& scalar)
    {
        return Vec3<T>(vec[0] + scalar, vec[1] + scalar, vec[2] + scalar);
    }

    template<typename T>
    Vec3<T> operator+ (const T& scalar, const Vec3<T>& vec)
    {
        return Vec3<T>(scalar + vec[0], scalar + vec[1], scalar + vec[2]);
    }

    template<typename T>
    Vec3<T> operator+ (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return Vec3<T>(vec1[0] + vec2[0], vec1[1] + vec2[1], vec1[2] + vec2[2]);
    }

    template<typename T>
    Vec3<T> operator- (const Vec3<T>& vec, const T& scalar)
    {
        return Vec3<T>(vec[0] - scalar, vec[1] - scalar, vec[2] - scalar);
    }

    template<typename T>
    Vec3<T> operator- (const T& scalar, const Vec3<T>& vec)
    {
        return Vec3<T>(scalar - vec[0], scalar - vec[1], scalar - vec[2]);
    }

    template<typename T>
    Vec3<T> operator- (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return Vec3<T>(vec1[0] - vec2[0], vec1[1] - vec2[1], vec1[2] - vec2[2]);
    }

    template<typename T>
    Vec3<T> operator* (const Vec3<T>& vec, const T& scalar)
    {
        return Vec3<T>(vec[0] * scalar, vec[1] * scalar, vec[2] * scalar);
    }

    template<typename T>
    Vec3<T> operator* (const T& scalar, const Vec3<T>& vec)
    {
        return Vec3<T>(scalar * vec[0], scalar * vec[1], scalar * vec[2]);
    }

    template<typename T>
    Vec3<T> operator* (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return Vec3<T>(vec1[0] * vec2[0], vec1[1] * vec2[1], vec1[2] * vec2[2]);
    }

    template<typename T>
    Vec3<T> operator/ (const Vec3<T>& vec, const T& scalar)
    {
        return Vec3<T>(vec[0] / scalar, vec[1] / scalar, vec[2] / scalar);
    }

    template<typename T>
    Vec3<T> operator/ (const T& scalar, const Vec3<T>& vec)
    {
        return Vec3<T>(scalar / vec[0], scalar / vec[1], scalar / vec[2]);
    }

    template<typename T>
    Vec3<T> operator/ (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return Vec3<T>(vec1[0] / vec2[0], vec1[1] / vec2[1], vec1[2] / vec2[2]);
    }

    template<typename T>
    bool operator== (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return vec1.mArray == vec2.mArray;
    }

    template<typename T>
    bool operator!= (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return vec1.mArray != vec2.mArray;
    }

    template<typename T>
    bool operator< (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return vec1.mArray < vec2.mArray;
    }

    template<typename T>
    bool operator<= (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return vec1.mArray <= vec2.mArray;
    }

    template<typename T>
    bool operator> (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return vec1.mArray > vec2.mArray;
    }

    template<typename T>
    bool operator>= (const Vec3<T>& vec1, const Vec3<T>& vec2)
    {
        return vec1.mArray >= vec2.mArray;
    }

    template<typename T>
    Vec3<T> operator- (const Vec3<T>& vec)
    {
        return Vec3<T>(-vec[0], -vec[1], -vec[2]);
    }

}