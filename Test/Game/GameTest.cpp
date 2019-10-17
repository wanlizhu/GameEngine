#define PREDEFINE_SETUP
#define PREDEFINE_WINDOWS_APP

#include <random>

#include "Setup.h"
#include "Algorithm.h"
#include "GLTF2Loader.h"

class GameSetup : public Setup
{
public:
    GameSetup()
    {
        gpGlobal->GetConfiguration<AppConfiguration>().SetAppName("Game Test");
        gpGlobal->GetConfiguration<AppConfiguration>().SetWidth(1080);
        gpGlobal->GetConfiguration<AppConfiguration>().SetHeight(1080);
        gpGlobal->GetConfiguration<GraphicsConfiguration>().SetDeviceType(eDevice_D3D11);
        gpGlobal->GetConfiguration<GraphicsConfiguration>().SetMSAA(eMSAA_4);

        auto& pWorld = gpGlobal->GetECSWorld();

        // Camera
        TransformComponent cameraTransformComp;
        CameraComponent cameraComp;
        cameraTransformComp.SetPosition(float3(3.0f, 0.0f, 3.0f));
        cameraTransformComp.SetRotate(float3(0.0f, 135.0f, 0.0f));
        cameraComp.SetBackground(float4(33.f / 255.f, 40.f / 255.f, 48.f / 255.f, 1.0f));
        auto pCamera = pWorld->CreateEntity<TransformComponent, CameraComponent>(cameraTransformComp, cameraComp);

        // Directional Light
        TransformComponent lightTransformComp;
        LightComponent lightComp;
        AnimationComponent lightAnimationComp;
        lightTransformComp.SetRotate(float3(0.0f, -135.0f, -45.0f));
        auto pDirectionalLight = std::make_shared<DirectionalLight>();
        lightComp.SetLight(pDirectionalLight);
        auto pLight = pWorld->CreateEntity<TransformComponent, LightComponent, AnimationComponent>(lightTransformComp, lightComp, lightAnimationComp);
        AnimationFunc func2 = [pLight](float elapsedTime) -> void
        {
            float second = elapsedTime / 1000;

            auto pTrans = pLight->GetComponent<TransformComponent>();
            auto rotate = pTrans->GetRotate();
            rotate.y -= second * 45.f;
            rotate.z = (std::abs(std::sinf(rotate.y*PI_F/180.f * 0.3f)) + 0.15f) * -60.0f;
            pTrans->SetRotate(rotate);
        };
        pLight->GetComponent<AnimationComponent>()->SetAnimationFunc(func2);

        GLTF2Loader loader;
        loader.Load("Asset/Scene/Test/DamagedHelmet/DamagedHelmet.gltf");
        loader.ApplyToWorld();

        /*// Plane
        TransformComponent planeTransformComp;
        MeshFilterComponent planeMeshFilterComp;
        MeshRendererComponent planeMeshRendererComp;
        planeTransformComp.SetPosition(float3(0.0f, -0.5f, 0.0f));
        auto pPlaneMesh = std::make_shared<PlaneMesh>();
        planeMeshFilterComp.SetMesh(pPlaneMesh);
        auto pPlane = pWorld->CreateEntity<TransformComponent, MeshFilterComponent, MeshRendererComponent>(planeTransformComp, planeMeshFilterComp, planeMeshRendererComp);

        // Entity 1
        TransformComponent cubeTransformComp1;
        MeshFilterComponent cubeMeshFilterComp1;
        MeshRendererComponent cubeMeshRendererComp1;
        AnimationComponent cubeAnimationComp1;
        cubeTransformComp1.SetPosition(float3(2.0f, 4.0f, -1.0f));
        cubeTransformComp1.SetRotate(float3(0.0f, 0.0f, 30.0f));
        auto pMesh = std::make_shared<CubeMesh>();
        cubeMeshFilterComp1.SetMesh(pMesh);
        auto pCube1 = pWorld->CreateEntity<TransformComponent, MeshFilterComponent, MeshRendererComponent, AnimationComponent>(cubeTransformComp1, cubeMeshFilterComp1, cubeMeshRendererComp1, cubeAnimationComp1);
        AnimationFunc func = [pCube1](float elapsedTime) -> void
        {
            float second = elapsedTime / 1000;

            auto pTrans = pCube1->GetComponent<TransformComponent>();
            auto rotate = pTrans->GetRotate();
            rotate.y += second * 45.f;
            pTrans->SetRotate(rotate);
        };
        pCube1->GetComponent<AnimationComponent>()->SetAnimationFunc(func);

        // Entity 2
        TransformComponent cubeTransformComp2;
        MeshFilterComponent cubeMeshFilterComp2;
        MeshRendererComponent cubeMeshRendererComp2;
        AnimationComponent cubeAnimationComp;
        cubeTransformComp2.SetPosition(float3(0.0f, 2.0f, 1.0f));
        cubeTransformComp2.SetRotate(float3(0.0f, -1.0f, 0.0f));
        cubeTransformComp2.SetScale(float3(2.0f, 4.0f, 2.0f));
        auto pCubeMesh2 = std::make_shared<CubeMesh>();
        cubeMeshFilterComp2.SetMesh(pCubeMesh2);
        auto pCube2 = pWorld->CreateEntity<TransformComponent, MeshFilterComponent, MeshRendererComponent>(cubeTransformComp2, cubeMeshFilterComp2, cubeMeshRendererComp2);*/
    }
};

static GameSetup setup;