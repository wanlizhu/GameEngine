#pragma once

namespace Engine
{
    template<typename T>
    Vec2<T>::Vec2() : mArray{T(), T()}
    {
    }

    template<typename T>
    Vec2<T>::Vec2(const Vec2& vec) : mArray{vec[0], vec[1]}
    {
    }

    template<typename T>
    Vec2<T>::Vec2(Vec2&& vec) : mArray(std::move(vec.mArray))
    {
    }

    template<typename T>
    Vec2<T>::Vec2(const T& val) : mArray{val, val}
    {
    }

    template<typename T>
    Vec2<T>::Vec2(const T& val1, const T& val2) : mArray{val1, val2}
    {
    }

    template<typename T>
    typename Vec2<T>::size_type Vec2<T>::Size() const
    {
        return mArray.size();
    }

    template<typename T>
    const T& Vec2<T>::operator[] (size_type index) const
    {
        return mArray[index];
    }

    template<typename T>
    T& Vec2<T>::operator[](size_type index)
    {
        return const_cast<T&>(static_cast<const Vec2&>(*this)[index]);
    }

    template<typename T>
    Vec2<T>& Vec2<T>::operator= (const Vec2& vec)
    {
        mArray = vec.mArray;
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec2<T>& Vec2<T>::operator= (const Vec2<U>& vec)
    {
        mArray[0] = static_cast<T>(vec[0]);
        mArray[1] = static_cast<T>(vec[1]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec2<T>& Vec2<T>::operator+= (const U& scalar)
    {
        mArray[0] += static_cast<T>(scalar);
        mArray[1] += static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec2<T>& Vec2<T>::operator+= (const Vec2<U>& vec)
    {
        mArray[0] += static_cast<T>(vec[0]);
        mArray[1] += static_cast<T>(vec[1]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec2<T>& Vec2<T>::operator-= (const U& scalar)
    {
        mArray[0] -= static_cast<T>(scalar);
        mArray[1] -= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec2<T>& Vec2<T>::operator-= (const Vec2<U>& vec)
    {
        mArray[0] -= static_cast<T>(vec[0]);
        mArray[1] -= static_cast<T>(vec[1]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec2<T>& Vec2<T>::operator*= (const U& scalar)
    {
        mArray[0] *= static_cast<T>(scalar);
        mArray[1] *= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec2<T>& Vec2<T>::operator*= (const Vec2<U>& vec)
    {
        mArray[0] *= static_cast<T>(vec[0]);
        mArray[1] *= static_cast<T>(vec[1]);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec2<T>& Vec2<T>::operator/= (const U& scalar)
    {
        mArray[0] /= static_cast<T>(scalar);
        mArray[1] /= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vec2<T>& Vec2<T>::operator/= (const Vec2<U>& vec)
    {
        mArray[0] /= static_cast<T>(vec[0]);
        mArray[1] /= static_cast<T>(vec[1]);
        return *this;
    }

    template<typename T>
    Vec2<T> operator+ (const Vec2<T>& vec, const T& scalar)
    {
        return Vec2<T>(vec[0] + scalar, vec[1] + scalar);
    }

    template<typename T>
    Vec2<T> operator+ (const T& scalar, const Vec2<T>& vec)
    {
        return Vec2<T>(scalar + vec[0], scalar + vec[1]);
    }

    template<typename T>
    Vec2<T> operator+ (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return Vec2<T>(vec1[0] + vec2[0], vec1[1] + vec2[1]);
    }

    template<typename T>
    Vec2<T> operator- (const Vec2<T>& vec, const T& scalar)
    {
        return Vec2<T>(vec[0] - scalar, vec[1] - scalar);
    }

    template<typename T>
    Vec2<T> operator- (const T& scalar, const Vec2<T>& vec)
    {
        return Vec2<T>(scalar - vec[0], scalar - vec[1]);
    }

    template<typename T>
    Vec2<T> operator- (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return Vec2<T>(vec1[0] - vec2[0], vec1[1] - vec2[1]);
    }

    template<typename T>
    Vec2<T> operator* (const Vec2<T>& vec, const T& scalar)
    {
        return Vec2<T>(vec[0] * scalar, vec[1] * scalar);
    }

    template<typename T>
    Vec2<T> operator* (const T& scalar, const Vec2<T>& vec)
    {
        return Vec2<T>(scalar * vec[0], scalar * vec[1]);
    }

    template<typename T>
    Vec2<T> operator* (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return Vec2<T>(vec1[0] * vec2[0], vec1[1] * vec2[1]);
    }

    template<typename T>
    Vec2<T> operator/ (const Vec2<T>& vec, const T& scalar)
    {
        return Vec2<T>(vec[0] / scalar, vec[1] / scalar);
    }

    template<typename T>
    Vec2<T> operator/ (const T& scalar, const Vec2<T>& vec)
    {
        return Vec2<T>(scalar / vec[0], scalar / vec[1]);
    }

    template<typename T>
    Vec2<T> operator/ (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return Vec2<T>(vec1[0] / vec2[0], vec1[1] / vec2[1]);
    }

    template<typename T>
    bool operator== (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return vec1.mArray == vec2.mArray;
    }

    template<typename T>
    bool operator!= (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return vec1.mArray != vec2.mArray;
    }

    template<typename T>
    bool operator< (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return vec1.mArray < vec2.mArray;
    }

    template<typename T>
    bool operator<= (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return vec1.mArray <= vec2.mArray;
    }

    template<typename T>
    bool operator> (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return vec1.mArray > vec2.mArray;
    }

    template<typename T>
    bool operator>= (const Vec2<T>& vec1, const Vec2<T>& vec2)
    {
        return vec1.mArray >= vec2.mArray;
    }

    template<typename T>
    Vec2<T> operator- (const Vec2<T>& vec)
    {
        return Vec2<T>(-vec[0], -vec[1]);
    }

}