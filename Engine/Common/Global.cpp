#include "Global.h"

#include "IApplication.h"
#include "IECSWorld.h"
#include "IRenderer.h"

#include "IEventSystem.h"
#include "IAnimationSystem.h"
#include "IDrawingSystem.h"
#include "ISceneSystem.h"
#include "IInputSystem.h"
#include "ILogSystem.h"

using namespace Engine;

namespace Engine
{
    Global* gpGlobal;
}

Global::Global()
{
}

Global::~Global()
{
    m_pRenderers.clear();
    m_pSystems.clear();
}

std::shared_ptr<IApplication> Global::GetApplication()
{
    return m_pApp;
}

std::shared_ptr<IECSWorld> Global::GetECSWorld()
{
    return m_pWorld;
}

std::shared_ptr<IEventSystem> Global::GetEventSystem()
{
    auto& pModule = GetRuntimeModule(eSystem_Event);
    auto pEventSystem = std::dynamic_pointer_cast<IEventSystem>(pModule);
    return pEventSystem;
}

std::shared_ptr<IAnimationSystem> Global::GetAnimationSystem()
{
    auto& pModule = GetRuntimeModule(eSystem_Animation);
    auto pAnimationSystem = std::dynamic_pointer_cast<IAnimationSystem>(pModule);
    return pAnimationSystem;
}

std::shared_ptr<IDrawingSystem> Global::GetDrawingSystem()
{
    auto& pModule = GetRuntimeModule(eSystem_Drawing);
    auto pDrawingSystem = std::dynamic_pointer_cast<IDrawingSystem>(pModule);
    return pDrawingSystem;
}

std::shared_ptr<ISceneSystem> Global::GetSceneSystem()
{
    auto& pModule = GetRuntimeModule(eSystem_Scene);
    auto pSceneSystem = std::dynamic_pointer_cast<ISceneSystem>(pModule);
    return pSceneSystem;
}

std::shared_ptr<IInputSystem> Global::GetInputSystem()
{
    auto& pModule = GetRuntimeModule(eSystem_Input);
    auto pInputSystem = std::dynamic_pointer_cast<IInputSystem>(pModule);
    return pInputSystem;
}

std::shared_ptr<ILogSystem> Global::GetLogSystem()
{
    auto& pModule = GetRuntimeModule(eSystem_Log);
    auto pLogSystem = std::dynamic_pointer_cast<ILogSystem>(pModule);
    return pLogSystem;
}

std::shared_ptr<IRenderer> Global::GetRenderer(ERendererType type)
{
    auto it = m_pRenderers.find(type);
    if (it == m_pRenderers.end())
        return nullptr;
    else
    {
        auto& renderer = it->second;
        auto result = std::dynamic_pointer_cast<IRenderer>(renderer);
        return result;
    }
}

Configuration& Global::GetConfiguration()
{
    return m_config;
}

FPSCounter& Global::GetFPSCounter()
{
    return m_fps;
}

std::shared_ptr<IECSSystem> Global::GetRuntimeModule(ESystemType e)
{
    auto it = m_pSystems.find(e);
    if (it == m_pSystems.end())
        return nullptr;
    else
    {
        auto& system = it->second;
        auto result = std::dynamic_pointer_cast<IECSSystem>(system);
        return result;

        
    }
}

Setup::Setup()
{
    if (gpGlobal == nullptr)
        gpGlobal = new Global();
}