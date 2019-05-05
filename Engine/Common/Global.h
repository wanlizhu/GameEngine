#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <stdint.h>

#include "Vec4.h"
#include "FPS.h"
#include "IECSWorld.h"
#include "ECSWorld.h"

namespace Engine
{
    enum ESystemType
    {
        eSystem_Event = 0,
        eSystem_Drawing = 1,
        eSystem_Scene = 2,
        eSystem_Input = 3,
        eSystem_Log = 4,
    };

    enum ERendererType
    {
        eRenderer_Start = 0,
        eRenderer_BasicPrim = 0,
        eRenderer_End = 1,
    };

    enum EDeviceType
    {
        eDevice_D3D11 = 0,
        eDevice_D3D12 = 1,
        eDevice_OGL = 2,
    };

    inline std::ostream& operator<< (std::ostream &out, EDeviceType const& d)
    {
        if (d == eDevice_D3D11)
            return out << "D3D11";
        else if (d == eDevice_D3D12)
            return out << "D3D12";
        else if (d == eDevice_OGL)
            return out << "OGL";
        else
            return out << "Unknown";
    }

    struct Configuration
    {
        Configuration(uint32_t _width = 1024,
                      uint32_t _height = 768,
                      EDeviceType _type = eDevice_D3D11,
                      const char* _appName="Game Engine") :
                      width(_width),
                      height(_height),
                      type(_type),
                      background(33.f / 255.f, 40.f / 255.f, 48.f / 255.f, 1.0f),
                      appName(_appName),
                      hWnd(nullptr)
        {}

        uint32_t width;
        uint32_t height;
        EDeviceType type;
        Vec4<float> background;
        const char* appName;
        void* hWnd;

        friend std::ostream& operator<< (std::ostream &out, Configuration const& c)
        {
            return out << "Configuration: " << std::endl <<
                          "app name: " << c.appName << std::endl <<
                          "width: " << c.width << std::endl <<
                          "height: " << c.height << std::endl <<
                          "device type: " << c.type << std::endl;
        }
    };

    class IApplication;
    class IEventSystem;
    class IDrawingSystem;
    class ISceneSystem;
    class IInputSystem;
    class ILogSystem;
    class IECSWorld;
    class IRenderer;

    class Global
    {
    public:
        Global();
        virtual ~Global();

        std::shared_ptr<IApplication> GetApplication();
        std::shared_ptr<IECSWorld> GetECSWorld();

        std::shared_ptr<IEventSystem> GetEventSystem();
        std::shared_ptr<IDrawingSystem> GetDrawingSystem();
        std::shared_ptr<ISceneSystem> GetSceneSystem();
        std::shared_ptr<IInputSystem> GetInputSystem();
        std::shared_ptr<ILogSystem> GetLogSystem();
        // Renderer
        std::shared_ptr<IRenderer> GetRenderer(ERendererType type);

        Configuration& GetConfiguration();
        FPSCounter& GetFPSCounter();

        template<typename T>
        void RegisterApp()
        {
            m_pWorld = std::make_shared<ECSWorld>();
            auto app = std::make_shared<T>();
            auto result = std::dynamic_pointer_cast<IApplication>(app);
            m_pApp = result;
        }

        template<typename T>
        void RegisterRuntimeModule(ESystemType e)
        {
            auto it = m_pSystems.find(e);
            if (it != m_pSystems.end())
                return;
            auto system = std::make_shared<T>();
            auto result = std::dynamic_pointer_cast<IECSSystem>(system);

            m_pSystems[e] = result;
        }

        template<typename T>
        void RegisterRenderer(ERendererType e)
        {
            auto it = m_pRenderers.find(e);
            if (it != m_pRenderers.end())
                return;
            auto renderer = std::make_shared<T>();
            auto result = std::dynamic_pointer_cast<IRenderer>(renderer);

            m_pRenderers[e] = result;
        }

    private:
        std::shared_ptr<IECSSystem> GetRuntimeModule(ESystemType e);

    private:
        std::shared_ptr<IECSWorld> m_pWorld;

        std::shared_ptr<IApplication> m_pApp;
        std::unordered_map<ESystemType, std::shared_ptr<IECSSystem>> m_pSystems;
        std::unordered_map<ERendererType, std::shared_ptr<IRenderer>> m_pRenderers;

        Configuration m_config;
        FPSCounter m_fps;
    };

    class Setup
    {
    public:
        Setup();
        virtual ~Setup() {};
    };

    extern Global* gpGlobal;
}