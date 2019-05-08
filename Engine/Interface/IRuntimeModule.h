#pragma once

namespace Engine
{
    class IRuntimeModule
    {
    public:
        virtual ~IRuntimeModule() {}

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual void Tick(float elapsedTime) = 0;
    };
}