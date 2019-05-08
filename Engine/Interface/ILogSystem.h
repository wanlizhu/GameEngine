#pragma once

#include "IRuntimeModule.h"

namespace Engine
{
    class ILogSystem : public IRuntimeModule
    {
    public:
        virtual ~ILogSystem() {}

        virtual void Initialize() override = 0;
        virtual void Shutdown() override = 0;

        virtual void Tick(float elapsedTime) override = 0;
    };
}