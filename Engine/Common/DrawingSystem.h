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

    private:
        bool EstablishConfiguration();
        bool PreConfiguration();
        bool CreateDevice();
        bool CreatePreResource();
        bool RegisterRenderer();
        bool PostConfiguration();

        void BuildFrameGraph();
        bool BuildForwardFrameGraph(std::shared_ptr<FrameGraph> pFrameGraph, std::shared_ptr<IEntity> pCamera);

        void GetVisableRenderable(RenderQueueItemListType& items);

        std::shared_ptr<DrawingTarget> CreateSwapChain();
        std::shared_ptr<DrawingDepthBuffer> CreateDepthBuffer();

        float4x4 UpdateViewMatrix(TransformComponent* pTransform);
        float4x4 UpdateProjectionMatrix(CameraComponent* pCamera);

    private:
        void* m_window;
        uint2 m_deviceSize;
        EConfigurationDeviceType m_deviceType;

        std::shared_ptr<DrawingDevice> m_pDevice;
        std::shared_ptr<DrawingContext> m_pContext;

        std::shared_ptr<DrawingEffectPool> m_pEffectPool;
        std::shared_ptr<DrawingResourceFactory> m_pResourceFactory;
        std::shared_ptr<DrawingResourceTable> m_pResourceTable;

        std::vector<std::shared_ptr<IEntity>> m_pCameraList;
        std::vector<std::shared_ptr<IEntity>> m_pMeshList;
    };
}