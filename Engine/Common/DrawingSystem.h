#pragma once

#include <memory>
#include <unordered_map>

#include "IDrawingSystem.h"

#include "Global.h"
#include "ECSSystem.h"

#include "Vector.h"
#include "Matrix.h"
#include "DrawingDevice.h"
#include "DrawingEffectPool.h"
#include "DrawingResourceTable.h"
#include "ForwardRenderer.h"
#include "FrameGraph.h"

namespace Engine
{
    class TransformComponent;
    class MeshFilterComponent;
    class MeshRendererComponent;
    class DrawingSystem : public IDrawingSystem, public ECSSystemBase<TransformComponent, MeshFilterComponent, MeshRendererComponent>
    {
    public:
        DrawingSystem();
        virtual ~DrawingSystem();

        void Initialize() override;
        void Shutdown() override;
        void Tick(float elapsedTime) override;

        void FlushEntity(std::shared_ptr<IEntity> pEntity) override;

        EConfigurationDeviceType GetDeviceType() const override;
        void SetDeviceType(EConfigurationDeviceType type) override;

        void FlipDebugState() override;

    private:
        bool EstablishConfiguration();
        bool PreConfiguration();
        bool CreateDevice();
        bool CreatePreResource();
        bool RegisterRenderer();
        bool PostConfiguration();

        void BuildFrameGraph(std::shared_ptr<IEntity> pCamera);
        bool BuildForwardFrameGraph(std::shared_ptr<FrameGraph> pFrameGraph, std::shared_ptr<IEntity> pCamera);
        bool BuildDeferredFrameGraph(std::shared_ptr<FrameGraph> pFrameGraph, std::shared_ptr<IEntity> pCamera);

        void GetVisableRenderable(RenderQueueItemListType& items);

        std::shared_ptr<DrawingTarget> CreateSwapChain();
        std::shared_ptr<DrawingDepthBuffer> CreateDepthBuffer();

        void GetViewMatrix(TransformComponent* pTransform, float4x4& view, float3& dir = float3());
        void GetProjectionMatrix(CameraComponent* pCamera, float4x4& proj);

        void GetLightViewProjectionMatrix(TransformComponent* pTransform, float4x4& view, float4x4& proj, float3& dir = float3());

        void UpdateCameraDir(float3 dir);
        void UpdateLightDir(float3 dir);
        void UpdateLightViewMatrix(float4x4 view);
        void UpdateLightProjMatrix(float4x4 proj);

    private:
        void* m_window;
        bool m_bDebug;
        uint2 m_deviceSize;
        EConfigurationDeviceType m_deviceType;

        std::shared_ptr<DrawingDevice> m_pDevice;
        std::shared_ptr<DrawingContext> m_pContext;

        std::shared_ptr<DrawingEffectPool> m_pEffectPool;
        std::shared_ptr<DrawingResourceFactory> m_pResourceFactory;
        std::shared_ptr<DrawingResourceTable> m_pResourceTable;

        std::vector<std::shared_ptr<IEntity>> m_pCameraList;
        std::vector<std::shared_ptr<IEntity>> m_pLightList;
        std::vector<std::shared_ptr<IEntity>> m_pMeshList;
    };
}