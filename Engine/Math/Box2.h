#pragma once

#include <algorithm>
#include <assert.h>

#include "Vec2.h"

namespace Engine
{
    class Box2
    {
    public:
        Vec2<float> mMin, mMax;

        const float MIN_BOX_BOUNDARY = 1e30f;
        const float MAX_BOX_BOUNDARY = -1e30f;

        Box2()
        {
            Clear();
        }

        Box2(const Vec2<float>& min, const Vec2<float>& max) : mMin(min), mMax(max)
        {
        }

        Box2(float minX, float minY, float maxX, float maxY) : mMin(minX, minY), mMax(maxX, maxY)
        {
        }

        Box2& operator=(const Box2& rhs)
        {
            if (this == &rhs)
                return *this;

            mMin = rhs.mMin;
            mMax = rhs.mMax;
            return *this;
        }

        void Set(float minX, float minY, float maxX, float maxY)
        {
            mMin.x = minX;
            mMin.y = minY;
        
            mMax.x = maxX;
            mMax.y = maxY;
        }

        Vec2<float> Size() const
        {
            return mMax - mMin;
        }

        Vec2<float> Center() const
        {
            return (mMin + mMax) * 0.5f;
        }

        float Radius() const
        {
            Vec2<float> distance = mMax - mMin;
            return 0.5f * std::sqrt(distance.x * distance.x + distance.y * distance.y);
        }

        float Width() const
        {
            assert(mMax.x >= mMin.x);
            return mMax.x - mMin.x;
        }

        float Height() const
        {
            assert(mMax.y >= mMin.y);
            return mMax.y - mMin.y;
        }

        bool IsEmpty() const
        {
            return mMin.x > MIN_BOX_BOUNDARY && mMax.x < MAX_BOX_BOUNDARY;
        }

        void Clear()
        {
            const Vec2<float> huge{ 2 * MIN_BOX_BOUNDARY, 2 * MAX_BOX_BOUNDARY };
            mMin = huge;
            mMax = -huge;
        }
    };
}