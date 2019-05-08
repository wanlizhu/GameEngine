#include "WindowsApplication.h"
#include "EventSystem.h"
#include "DrawingSystem.h"
#include "SceneSystem.h"
#include "InputSystem.h"
#include "LogSystem.h"

#include "CameraComponent.h"
#include "MeshFilterComponent.h"
#include "MeshRendererComponent.h"
#include "TransformComponent.h"

#include "CubeMesh.h"
#include "GLTF2Mesh.h"

#include "ForwardRenderer.h"

using namespace Engine;
using namespace Platform;
    
class GameSetup : public Setup
{
public:
    GameSetup()
    {
        gpGlobal->GetConfiguration().appName = "Game Test";
        gpGlobal->GetConfiguration().width = 1080;
        gpGlobal->GetConfiguration().height = 1080;
        gpGlobal->GetConfiguration().type = eDevice_D3D11;

        gpGlobal->RegisterApp<WindowsApplication>();

        gpGlobal->RegisterRuntimeModule<InputSystem>(eSystem_Input);
        gpGlobal->RegisterRuntimeModule<EventSystem>(eSystem_Event);
        gpGlobal->RegisterRuntimeModule<SceneSystem>(eSystem_Scene);
        gpGlobal->RegisterRuntimeModule<DrawingSystem>(eSystem_Drawing);
        gpGlobal->RegisterRuntimeModule<LogSystem>(eSystem_Log);

        gpGlobal->RegisterRenderer<ForwardRenderer>(eRenderer_Forward);

        auto& pWorld = gpGlobal->GetECSWorld();

        // ECSSystem
        pWorld->AddECSSystem(gpGlobal->GetInputSystem());
        pWorld->AddECSSystem(gpGlobal->GetEventSystem());
        pWorld->AddECSSystem(gpGlobal->GetDrawingSystem());
        pWorld->AddECSSystem(gpGlobal->GetLogSystem());

        // Entity
        TransformComponent entityPosComp;
        MeshFilterComponent meshFilterComp;
        MeshRendererComponent meshRendererComp;

        entityPosComp.SetPosition(Vec3<float>(1.0f, 1.0f, 1.0f));

        //auto pMesh = std::make_shared<CubeMesh>();
        auto pMesh = std::make_shared<GLTF2Mesh>("Asset/Scene/Test/DamagedHelmet.gltf");
        meshFilterComp.SetMesh(pMesh);

        auto entity = pWorld->CreateEntity<TransformComponent, MeshFilterComponent, MeshRendererComponent>(entityPosComp, meshFilterComp, meshRendererComp);

        // Camera
        TransformComponent cameraPosComp;
        CameraComponent cameraComp;

        cameraPosComp.SetPosition(Vec3<float>(0.0f, 0.0f, -5.0f));

        auto camera = pWorld->CreateEntity<TransformComponent, CameraComponent>(cameraPosComp, cameraComp);

    }
};

static GameSetup setup;