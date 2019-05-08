#pragma once

#include <memory>
#include <unordered_map>

#include "IDrawingSystem.h"

#include "Global.h"
#include "ECSSystem.h"

#include "DrawingDevice.h"
#include "DrawingEffectPool.h"
#include "DrawingResourceTable.h"
#include "DrawingType.h"
#include "ForwardRenderer.h"

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

        void BeginFrame() override;
        void EndFrame() override;

        EDeviceType GetDeviceType() const override;
        void SetDeviceType(EDeviceType type) override;

    private:
        bool EstablishConfiguration();
        bool PreConfiguration();
        bool CreateDevice();
        bool CreatePreResource();
        bool RegisterRenderer();
        bool PostConfiguration();

        std::shared_ptr<DrawingTarget> CreateSwapChain();
        std::shared_ptr<DrawingDepthBuffer> CreateDepthBuffer();

        float4x4 UpdateWorldMatrix(TransformComponent* pTransform);
        float4x4 UpdateViewMatrix(TransformComponent* pTransform);
        float4x4 UpdateProjectionMatrix(CameraComponent* pCamera);

    private:
        void* m_window;
        uint2 m_deviceSize;
        EDeviceType m_deviceType;

        std::shared_ptr<DrawingDevice> m_pDevice;
        std::shared_ptr<DrawingContext> m_pContext;

        std::shared_ptr<DrawingEffectPool> m_pEffectPool;
        std::shared_ptr<DrawingResourceFactory> m_pResourceFactory;
        std::shared_ptr<DrawingResourceTable> m_pResourceTable;

        std::vector<std::shared_ptr<IEntity>> m_pCameraList;
        std::vector<std::shared_ptr<IEntity>> m_pMeshList;

        typedef std::unordered_map<ERendererType, std::shared_ptr<IRenderer>> RendererTable;
        RendererTable m_rendererTable;
    };
}