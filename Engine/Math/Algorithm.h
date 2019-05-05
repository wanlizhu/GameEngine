#pragma once

#include <algorithm>

namespace Engine
{
    template <typename T>
    inline T AddBit(T& mask, uint32_t bit)
    {
        mask |= (T)1 << (T)bit;
        return mask;
    }

    template <typename T>
    inline T ClearBit(T& mask, uint32_t bit)
    {
        mask &= ~((T)1 << (T)bit);
        return mask;
    }
    /***************************************************************************
    * These functions were taken from the MiniEngine.
    * Source code available here:
    * https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Math/Common.h
    * Retrieved: January 13, 2016
    **************************************************************************/
    template <typename T>
    inline T AlignUpWithMask(T value, uint64_t mask)
    {
        return (T)(((uint64_t)value + mask) & ~mask);
    }

    template <typename T>
    inline T AlignDownWithMask(T value, uint64_t mask)
    {
        return (T)((uint64_t)value & ~mask);
    }

    template <typename T>
    inline T AlignUp(T value, uint64_t alignment)
    {
        return AlignUpWithMask(value, alignment - 1);
    }

    template <typename T>
    inline T AlignDown(T value, uint64_t alignment)
    {
        return AlignDownWithMask(value, alignment - 1);
    }

    template <typename T>
    inline bool IsAligned(T value, uint64_t alignment)
    {
        return 0 == ((uint64_t)value & (alignment - 1));
    }

    template <typename T>
    inline T DivideByMultiple(T value, uint64_t alignment)
    {
        return (T)((value + alignment - 1) / alignment);
    }
}
