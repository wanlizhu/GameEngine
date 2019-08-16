#pragma once

#include <memory>
#include <string>

#include "IRenderer.h"
#include "DrawingPass.h"
#include "FrameGraph.h"

#include "RenderQueue.h"
#include "DrawingStreamedResource.h"

namespace Engine
{
    class BaseRenderer : public IRenderer
    {
    public:
        BaseRenderer();
        virtual ~BaseRenderer() {}

        virtual void DefineResources(DrawingResourceTable& resTable) override = 0;
        virtual void SetupBuffers(DrawingResourceTable& resTable) override = 0;

        void Begin() override;
        void AddRenderables(RenderQueueItemListType renderables) override;

        void Clear(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass) override;
        void Flush(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass) override;

        void AttachDevice(const std::shared_ptr<DrawingDevice>& pDevice, const std::shared_ptr<DrawingContext>& pContext) override;
        void AttachMesh(const IMesh* pMesh) override;

        void CreateDataResources(DrawingResourceTable& resTable) override;
        virtual void BuildPass() = 0;
        std::shared_ptr<DrawingPass> GetPass(std::shared_ptr<std::string> pName) override;

    private:
        virtual void BeginDrawPass() = 0;
        virtual void EndDrawPass() = 0;

        virtual void FlushData() = 0;
        virtual void ResetData() = 0;
        virtual void UpdatePrimitive(DrawingResourceTable& resTable) = 0;

        float4x4 UpdateWorldMatrix(const TransformComponent* pTransform);

    public:
        // Vertex format names
        FuncResourceName(VertexFormatP)
        FuncResourceName(VertexFormatPN)
        // Vertex buffer names
        FuncResourceName(DefaultStaticPositionBuffer)
        FuncResourceName(DefaultStaticNormalBuffer)
        FuncResourceName(DefaultDynamicPositionBuffer)
        FuncResourceName(DefaultDynamicNormalBuffer)
        // Index buffer names
        FuncResourceName(DefaultStaticIndexBuffer)
        FuncResourceName(DefaultDynamicIndexBuffer)
        // Constant buffer names
        FuncResourceName(DefaultWorldMatrix)
        FuncResourceName(DefaultViewMatrix)
        FuncResourceName(DefaultProjectionMatrix)
        // Render target names
        FuncResourceName(ShadowMapTarget)
        FuncResourceName(DefaultTarget)
        FuncResourceName(DefaultDepthBuffer)
        FuncResourceName(ScreenTarget)
        FuncResourceName(ScreenDepthBuffer)
        // Render state names
        FuncResourceName(DefaultDepthState)
        FuncResourceName(DefaultBlendState)
        FuncResourceName(DefaultRasterState)
        // Varing states names
        FuncResourceName(DefaultVaringStates)
        // Primitive names
        FuncResourceName(DefaultPrimitive)

    protected:
        void DefineDefaultResources(DrawingResourceTable& resTable);

        void DefineGeneralEffect(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pSourceName, std::shared_ptr<std::string> pTechName, DrawingResourceTable& resTable);
        void DefineLinkedEffect(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pVSName, std::shared_ptr<std::string> pPSName, DrawingResourceTable& resTable);

        void DefinePipelineState(std::shared_ptr<std::string> pPipelineStateName,
                                 std::shared_ptr<std::string> pVertexFormatName,
                                 std::shared_ptr<std::string> pPrimitiveName,
                                 std::shared_ptr<std::string> pEffectName,
                                 std::shared_ptr<std::string> pBlendStateName,
                                 std::shared_ptr<std::string> pRasterStateName,
                                 std::shared_ptr<std::string> pDepthStencilStateName,
                                 std::shared_ptr<std::string> pRenderTargetName,
                                 DrawingResourceTable& resTable);

        void DefineVertexShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable);
        void DefinePixelShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable);
        void DefineVertexShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable);
        void DefinePixelShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable);

        void DefineVertexFormatP(DrawingResourceTable& resTable);
        void DefineVertexFormatPN(DrawingResourceTable& resTable);
        void DefineStaticVertexBuffer(std::shared_ptr<std::string> pName, uint32_t stride, uint32_t count, const void* data, uint32_t size, DrawingResourceTable& resTable);
        void DefineStaticIndexBuffer(std::shared_ptr<std::string> pName, uint32_t count, const void* data, uint32_t size, DrawingResourceTable& resTable);

        void DefineDynamicVertexBuffer(std::shared_ptr<std::string> pName, uint32_t stride, uint32_t count, DrawingResourceTable& resTable);
        void DefineDynamicIndexBuffer(std::shared_ptr<std::string> pName, uint32_t count, DrawingResourceTable& resTable);

        void DefineWorldMatrixConstantBuffer(DrawingResourceTable& resTable);
        void DefineViewMatrixConstantBuffer(DrawingResourceTable& resTable);
        void DefineProjectionMatrixConstantBuffer(DrawingResourceTable& resTable);

        void DefineDefaultDepthState(DrawingResourceTable& resTable);
        void DefineDefaultBlendState(DrawingResourceTable& resTable);
        void DefineDefaultRasterState(DrawingResourceTable& resTable);

        void DefineTarget(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);
        void DefineDepthBuffer(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);

        void DefineExternalTarget(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);
        void DefineExternalDepthBuffer(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);
        void DefineExternalTexture(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);

        bool DefineDynamicTexture(std::shared_ptr<std::string> pName, EDrawingFormatType format, uint32_t elementCount, DrawingResourceTable& resTable);
        void DefineDynamicTextureWithInit(std::shared_ptr<std::string> pName, EDrawingFormatType format, uint32_t elementCount, void* pData, uint32_t size, DrawingResourceTable& resTable);
        void DefineVaringStates(DrawingResourceTable& resTable);
        void DefinePrimitive(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);

        void BindResource(DrawingPass& pass, std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName);
        void BindEffect(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindVertexFormat(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindVertexBuffer(DrawingPass& pass, uint32_t index, std::shared_ptr<std::string> pName);
        void BindIndexBuffer(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindTarget(DrawingPass& pass, uint32_t index, std::shared_ptr<std::string> pName);
        void BindDepthBuffer(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindDepthState(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindBlendState(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindRasterState(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindPrimitive(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindVaringStates(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindPipelineState(DrawingPass& pass, std::shared_ptr<std::string> pName);

        void AddConstantSlot(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void AddTextureSlot(DrawingPass& pass, std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pParamName);

        void BindStaticInputsP(DrawingPass& pass);
        void BindDynamicInputsP(DrawingPass& pass);
        void BindStaticInputsPN(DrawingPass& pass);
        void BindDynamicInputsPN(DrawingPass& pass);
        void BindStates(DrawingPass& pass);
        void BindOutput(DrawingPass& pass);

        void BindConstants(DrawingPass& pass);

        std::shared_ptr<DrawingTransientTexture> CreateTransientTexture(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);
        std::shared_ptr<DrawingPersistTexture> CreatePersistTexture(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);

        std::shared_ptr<DrawingTransientVertexBuffer> CreateTransientVertexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);
        std::shared_ptr<DrawingPersistVertexBuffer> CreatePersistVertexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);

        std::shared_ptr<DrawingTransientIndexBuffer> CreateTransientIndexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);
        std::shared_ptr<DrawingPersistIndexBuffer> CreatePersistIndexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);

        std::shared_ptr<DrawingPass> CreatePass(std::shared_ptr<std::string> pName);

    private:
        template<typename T>
        void DoDefineShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable);

        template<typename T>
        void DoDefineShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable);

    protected:
        static const uint32_t DYNAMIC_TEX_ROW_SIZE = 1024;

        static const uint32_t MAX_VERTEX_COUNT = 65536 * 4;
        static const uint32_t MAX_INDEX_COUNT = 65536 * 4;

        static const uint32_t PositionOffset = sizeof(float3);
        static const uint32_t NormalOffset = sizeof(float3);

        std::shared_ptr<DrawingTransientVertexBuffer> m_pTransientPositionBuffer;
        std::shared_ptr<DrawingTransientVertexBuffer> m_pTransientNormalBuffer;
        std::shared_ptr<DrawingTransientIndexBuffer> m_pTransientIndexBuffer;

        std::shared_ptr<DrawingDevice> m_pDevice;
        std::shared_ptr<DrawingContext> m_pDeviceContext;

        typedef std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<DrawingPass>> DrawingPassTable;

        DrawingPassTable m_passTable;
        RenderQueue m_renderQueue;
    };

    template<typename T>
    void BaseRenderer::DoDefineShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable)
    {
        auto pDesc = std::make_shared<T>();

        pDesc->mProgramType = eProgram_Binary;
        pDesc->mpName = pShaderName;
        pDesc->mpSourceName = pSourceName;

        resTable.AddResourceEntry(pShaderName, pDesc);
    }

    template<typename T>
    void BaseRenderer::DoDefineShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable)
    {
        auto pDesc = std::make_shared<T>();

        pDesc->mProgramType = eProgram_File;
        pDesc->mpName = pShaderName;
        pDesc->mpSourceName = pFileName;
        pDesc->mpEntryName = pEntryName;

        resTable.AddResourceEntry(pShaderName, pDesc);
    }
}