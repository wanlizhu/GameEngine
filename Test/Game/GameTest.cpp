#include <random>

#include "WindowsApplication.h"
#include "EventSystem.h"
#include "AnimationSystem.h"
#include "DrawingSystem.h"
#include "SceneSystem.h"
#include "InputSystem.h"
#include "LogSystem.h"

#include "AnimationComponent.h"
#include "CameraComponent.h"
#include "MeshFilterComponent.h"
#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "PolylineRendererComponent.h"

#include "CubeMesh.h"
#include "GLTF2Mesh.h"

#include "ForwardRenderer.h"
#include "PolylineRenderer.h"

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
        gpGlobal->RegisterRuntimeModule<AnimationSystem>(eSystem_Animation);
        gpGlobal->RegisterRuntimeModule<DrawingSystem>(eSystem_Drawing);
        gpGlobal->RegisterRuntimeModule<LogSystem>(eSystem_Log);

        //gpGlobal->RegisterRenderer<ForwardRenderer>(eRenderer_Forward);
        gpGlobal->RegisterRenderer<PolylineRenderer>(eRenderer_Polyline);

        auto& pWorld = gpGlobal->GetECSWorld();

        // ECSSystem
        pWorld->AddECSSystem(gpGlobal->GetInputSystem());
        pWorld->AddECSSystem(gpGlobal->GetEventSystem());
        pWorld->AddECSSystem(gpGlobal->GetAnimationSystem());
        pWorld->AddECSSystem(gpGlobal->GetDrawingSystem());
        pWorld->AddECSSystem(gpGlobal->GetLogSystem());

        // Entity
        TransformComponent entityPosComp;
        MeshFilterComponent meshFilterComp;
        MeshRendererComponent meshRendererComp;
        AnimationComponent animationComp;
        entityPosComp.SetPosition(Vec3<float>(1.0f, 1.0f, 1.0f));
        auto pMesh = std::make_shared<CubeMesh>();
        //auto pMesh = std::make_shared<GLTF2Mesh>("Asset/Scene/Test/DamagedHelmet.gltf");
        meshFilterComp.SetMesh(pMesh);
        auto entity = pWorld->CreateEntity<TransformComponent, MeshFilterComponent, MeshRendererComponent, AnimationComponent>(entityPosComp, meshFilterComp, meshRendererComp, animationComp);
        AnimationComponent::AnimationFunc func = [entity](float elapsedTime) -> void
        {
            float second = elapsedTime / 1000;

            auto pTrans = entity->GetComponent<TransformComponent>();
            auto rotate = pTrans->GetRotate();
            rotate.y += second * 1.f;
            pTrans->SetRotate(rotate);
        };
        entity->GetComponent<AnimationComponent>()->SetAnimationFunc(func);

        // Camera
        TransformComponent cameraPosComp;
        CameraComponent cameraComp;
        cameraPosComp.SetPosition(Vec3<float>(0.0f, 2.0f, -5.0f));
        auto camera = pWorld->CreateEntity<TransformComponent, CameraComponent>(cameraPosComp, cameraComp);

        // LineSegment
        PolylineRendererComponent polylineRendererComp;
        PolylineRendererComponent::LineSegmentGeom geom;

        const int LINE_TEST_COUNT = 1000;

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> distWidth(0, gpGlobal->GetConfiguration().width);
        std::uniform_int_distribution<std::mt19937::result_type> distHeight(0, gpGlobal->GetConfiguration().height);

        std::uniform_int_distribution<std::mt19937::result_type> distColor(0, 255);

        for (uint32_t i = 0; i < LINE_TEST_COUNT; i ++)
        {
            geom.startPoint = float2((float)distWidth(rng), (float)distHeight(rng));
            geom.endPoint = float2((float)distWidth(rng), (float)distHeight(rng));
            geom.color = pack_color(uint4(distColor(rng), distColor(rng), distColor(rng), 255));
            geom.drawZ = 1.0f;
            polylineRendererComp.SetGeometry(geom);
            auto segment = pWorld->CreateEntity<PolylineRendererComponent>(polylineRendererComp);
        }
    }

private:
    uint32_t pack_color(uint4 color)
    {
        return (color.z & 0x000000ff) |
        ((color.y & 0x000000ff) << 8) |
        ((color.x & 0x000000ff) << 16) |
        ((color.w & 0x000000ff) << 24);
    }

};

static GameSetup setup;