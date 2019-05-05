#pragma once

#include <memory>
#include "IRuntimeModule.h"

namespace Engine
{
    class IApplication : public IRuntimeModule
    {
    public:
        virtual ~IApplication() {}

        virtual void Initialize() override = 0;
        virtual void Shutdown() override = 0;

        virtual void Tick() override = 0;

        virtual bool IsQuit() const = 0;
    };
}