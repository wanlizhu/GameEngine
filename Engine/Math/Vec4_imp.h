#pragma once

namespace Engine
{
    template<typename T>
    Vec4<T>::Vec4() : mArray{T(), T(), T(), T()}
    {
    }

    template<typename T>
    Vec4<T>::Vec4(const Vec4& vec) : mArray{vec[0], vec[1], vec[2], vec[3]}
    {
    }

    template<typename T>
    Vec4<T>::Vec4(Vec4&& vec) : mArray(std::move(vec.mArray))
    {
    }

    template<typename T>
    Vec4<T>::Vec4(const T& val) : mArray{val, val, val, val}
    {
    }

    template<typename T>
    Vec4<T>::Vec4(const T& val1, const T& val2, const T& val3, const T& val4) : mArray{val1, val2, val3, val4}
    {
    }

    template<typename T>
    typename Vec4<T>::size_type Vec4<T>::Size() const
    {
        return mArray.size();
    }

    template<typename T>
    const T& Vec4<T>::operator[] (size_type index) const
    {
        return mArray[index];
    }

    template<typename T>
    T& Vec4<T>::operator[](size_type index)
    {
        return const_cast<T&>(static_cast<const Vec4&>(*this)[index]);
    }

    template<typename T>
    Vec4<T>& Vec4<T>::operator= (const Vec4& vec)
    {
        mArray = vec.mArray;
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec4<T>& Vec4<T>::operator= (const Vec4<U>& vec)
    {
        mArray[0] = static_cast<T>(vec[0]);
        mArray[1] = static_cast<T>(vec[1]);
        mArray[2] = static_cast<T>(vec[2]);
        mArray[3] = static_cast<T>(vec[3]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec4<T>& Vec4<T>::operator+= (const U& scalar)
    {
        mArray[0] += static_cast<T>(scalar);
        mArray[1] += static_cast<T>(scalar);
        mArray[2] += static_cast<T>(scalar);
        mArray[2] += static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec4<T>& Vec4<T>::operator+= (const Vec4<U>& vec)
    {
        mArray[0] += static_cast<T>(vec[0]);
        mArray[1] += static_cast<T>(vec[1]);
        mArray[2] += static_cast<T>(vec[2]);
        mArray[3] += static_cast<T>(vec[3]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec4<T>& Vec4<T>::operator-= (const U& scalar)
    {
        mArray[0] -= static_cast<T>(scalar);
        mArray[1] -= static_cast<T>(scalar);
        mArray[2] -= static_cast<T>(scalar);
        mArray[3] -= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec4<T>& Vec4<T>::operator-= (const Vec4<U>& vec)
    {
        mArray[0] -= static_cast<T>(vec[0]);
        mArray[1] -= static_cast<T>(vec[1]);
        mArray[2] -= static_cast<T>(vec[2]);
        mArray[3] -= static_cast<T>(vec[3]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec4<T>& Vec4<T>::operator*= (const U& scalar)
    {
        mArray[0] *= static_cast<T>(scalar);
        mArray[1] *= static_cast<T>(scalar);
        mArray[2] *= static_cast<T>(scalar);
        mArray[3] *= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec4<T>& Vec4<T>::operator*= (const Vec4<U>& vec)
    {
        mArray[0] *= static_cast<T>(vec[0]);
        mArray[1] *= static_cast<T>(vec[1]);
        mArray[2] *= static_cast<T>(vec[2]);
        mArray[3] *= static_cast<T>(vec[3]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec4<T>& Vec4<T>::operator/= (const U& scalar)
    {
        mArray[0] /= static_cast<T>(scalar);
        mArray[1] /= static_cast<T>(scalar);
        mArray[2] /= static_cast<T>(scalar);
        mArray[3] /= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec4<T>& Vec4<T>::operator/= (const Vec4<U>& vec)
    {
        mArray[0] /= static_cast<T>(vec[0]);
        mArray[1] /= static_cast<T>(vec[1]);
        mArray[2] /= static_cast<T>(vec[2]);
        mArray[3] /= static_cast<T>(vec[3]);
        return *this;
    }

    template<typename T>
    Vec4<T> operator+ (const Vec4<T>& vec, const T& scalar)
    {
        return Vec4<T>(vec[0] + scalar, vec[1] + scalar, vec[2] + scalar, vec[3] + scalar);
    }

    template<typename T>
    Vec4<T> operator+ (const T& scalar, const Vec4<T>& vec)
    {
        return Vec4<T>(scalar + vec[0], scalar + vec[1], scalar + vec[2], scalar + vec[3]);
    }

    template<typename T>
    Vec4<T> operator+ (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return Vec4<T>(vec1[0] + vec2[0], vec1[1] + vec2[1], vec1[2] + vec2[2], vec1[3] + vec2[3]);
    }

    template<typename T>
    Vec4<T> operator- (const Vec4<T>& vec, const T& scalar)
    {
        return Vec4<T>(vec[0] - scalar, vec[1] - scalar, vec[2] - scalar, vec[3] - scalar);
    }

    template<typename T>
    Vec4<T> operator- (const T& scalar, const Vec4<T>& vec)
    {
        return Vec4<T>(scalar - vec[0], scalar - vec[1], scalar - vec[2], scalar - vec[3]);
    }

    template<typename T>
    Vec4<T> operator- (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return Vec4<T>(vec1[0] - vec2[0], vec1[1] - vec2[1], vec1[2] - vec2[2], vec1[3] - vec2[3]);
    }

    template<typename T>
    Vec4<T> operator* (const Vec4<T>& vec, const T& scalar)
    {
        return Vec4<T>(vec[0] * scalar, vec[1] * scalar, vec[2] * scalar, vec[3] * scalar);
    }

    template<typename T>
    Vec4<T> operator* (const T& scalar, const Vec4<T>& vec)
    {
        return Vec4<T>(scalar * vec[0], scalar * vec[1], scalar * vec[2], scalar * vec[3]);
    }

    template<typename T>
    Vec4<T> operator* (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return Vec4<T>(vec1[0] * vec2[0], vec1[1] * vec2[1], vec1[2] * vec2[2], vec1[3] * vec2[3]);
    }

    template<typename T>
    Vec4<T> operator/ (const Vec4<T>& vec, const T& scalar)
    {
        return Vec4<T>(vec[0] / scalar, vec[1] / scalar, vec[2] / scalar, vec[3] / scalar);
    }

    template<typename T>
    Vec4<T> operator/ (const T& scalar, const Vec4<T>& vec)
    {
        return Vec4<T>(scalar / vec[0], scalar / vec[1], scalar / vec[2], scalar / vec[3]);
    }

    template<typename T>
    Vec4<T> operator/ (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return Vec4<T>(vec1[0] / vec2[0], vec1[1] / vec2[1], vec1[2] / vec2[2], vec1[3] / vec2[3]);
    }

    template<typename T>
    bool operator== (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return vec1.mArray == vec2.mArray;
    }

    template<typename T>
    bool operator!= (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return vec1.mArray != vec2.mArray;
    }

    template<typename T>
    bool operator< (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return vec1.mArray < vec2.mArray;
    }

    template<typename T>
    bool operator<= (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return vec1.mArray <= vec2.mArray;
    }

    template<typename T>
    bool operator> (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return vec1.mArray > vec2.mArray;
    }

    template<typename T>
    bool operator>= (const Vec4<T>& vec1, const Vec4<T>& vec2)
    {
        return vec1.mArray >= vec2.mArray;
    }

    template<typename T>
    Vec4<T> operator- (const Vec4<T>& vec)
    {
        return Vec4<T>(-vec[0], -vec[1], -vec[2], -vec[3]);
    }

}