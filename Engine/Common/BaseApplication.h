#pragma once

#include <string>

#include "IApplication.h"

namespace Engine
{
    enum ESystemEvent
    {
        eEv_System_App = 1,
    };

    class IDrawingSystem;
    class ISceneSystem;
    class IInputSystem;
    class ILog;
    class BaseApplication : public IApplication
    {
    public:
        BaseApplication() {}
        virtual ~BaseApplication() {}

        void Initialize() override;
        void Shutdown() override;

        void Tick(float elapsedTime) override;

        bool IsQuit() const override;

    protected:
        static bool m_bQuit;
        std::shared_ptr<IECSWorld> m_pWorld;
    };
}