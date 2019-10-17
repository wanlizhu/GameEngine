#pragma once

#include "Global.h"

namespace Engine
{
    class IDrawingSystem
    {
    public:
        IDrawingSystem() = default;
        virtual ~IDrawingSystem() = default;

        virtual EConfigurationDeviceType GetDeviceType() const = 0;
        virtual void SetDeviceType(EConfigurationDeviceType type) = 0;

        virtual void FlipDebugState() = 0;
    };
}