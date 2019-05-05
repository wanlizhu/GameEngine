#pragma once

#include "Global.h"

namespace Engine
{
    class IDrawingSystem
    {
    public:
        IDrawingSystem() = default;
        virtual ~IDrawingSystem() = default;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual EDeviceType GetDeviceType() const = 0;
        virtual void SetDeviceType(EDeviceType type) = 0;
    };
}