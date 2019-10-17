#pragma once

#ifdef PREDEFINE_SETUP
    #include "Global.h"

    #include "EventSystem.h"
    #include "InputSystem.h"
    #include "LogSystem.h"

    using namespace Engine;
    #ifdef PREDEFINE_WINDOWS_APP
        #include "WindowsApplication.h"
        #define PREDEFINE_APP
    #endif

    #ifdef PREDEFINE_APP
        #include "AnimationSystem.h"
        #include "DrawingSystem.h"
        #include "SceneSystem.h"

        #include "AnimationComponent.h"
        #include "CameraComponent.h"
        #include "LightComponent.h"
        #include "MeshFilterComponent.h"
        #include "MeshRendererComponent.h"
        #include "TransformComponent.h"

        #include "CubeMesh.h"
        #include "PlaneMesh.h"

        #include "DirectionalLight.h"

        #include "ForwardRenderer.h"

        using namespace Platform;
        std::vector<AnimationFunc> AnimationSystem::s_cbTables;
    #endif
    IComponent::CompTableType IComponent::m_compTable;

    class Setup
    {
    public:
        Setup()
        {
            if (gpGlobal == nullptr)
                gpGlobal = new Global();

    #ifdef PREDEFINE_APP
            gpGlobal->RegisterApp<WindowsApplication>();
    #else
            gpGlobal->RegisterApp<BaseApplication>();
    #endif

            gpGlobal->RegisterRuntimeModule<InputSystem>(eSystem_Input);
            gpGlobal->RegisterRuntimeModule<EventSystem>(eSystem_Event);
            gpGlobal->RegisterRuntimeModule<LogSystem>(eSystem_Log);
    #ifdef PREDEFINE_APP
            gpGlobal->RegisterRuntimeModule<SceneSystem>(eSystem_Scene);
            gpGlobal->RegisterRuntimeModule<AnimationSystem>(eSystem_Animation);
            gpGlobal->RegisterRuntimeModule<DrawingSystem>(eSystem_Drawing);
    #endif

            auto& pWorld = gpGlobal->GetECSWorld();

            pWorld->AddECSSystem(gpGlobal->GetInputSystem());
            pWorld->AddECSSystem(gpGlobal->GetEventSystem());
            pWorld->AddECSSystem(gpGlobal->GetLogSystem());
    #ifdef PREDEFINE_APP
            pWorld->AddECSSystem(gpGlobal->GetAnimationSystem());
            pWorld->AddECSSystem(gpGlobal->GetDrawingSystem());

            gpGlobal->RegisterRenderer<ForwardRenderer>(eRenderer_Forward);
    #endif
        }

        virtual ~Setup()
        {
        }
    };

#endif