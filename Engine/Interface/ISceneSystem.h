#pragma once

#include "IRuntimeModule.h"

namespace Engine
{
    class ISceneSystem : public IRuntimeModule
    {
    public:
        ISceneSystem() = default;
        virtual ~ISceneSystem() = default;

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual void Tick(float elapsedTime) = 0;
    };
}