#pragma once

#include <memory>
#include <string>

#include "IRenderer.h"
#include "DrawingPass.h"
#include "FrameGraph.h"

#include "RenderQueue.h"
#include "DrawingStreamedResource.h"
#include "DrawingTextureTarget.h"

namespace Engine
{
    class BaseRenderer : public IRenderer
    {
    public:
        BaseRenderer();
        virtual ~BaseRenderer() {}

        virtual void DefineResources(DrawingResourceTable& resTable) override = 0;
        virtual void SetupBuffers(DrawingResourceTable& resTable) override = 0;

        void AddRenderables(RenderQueueItemListType renderables) override;

        void Clear(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass) override;
        void Render(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass) override;
        void RenderRect(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass) override;
        void CopyRect(DrawingResourceTable& resTable, std::shared_ptr<std::string> pSrcName, std::shared_ptr<std::string> pDstName, const int2& dstOrigin) override;

        void AttachDevice(const std::shared_ptr<DrawingDevice>& pDevice, const std::shared_ptr<DrawingContext>& pContext) override;
        void AttachMesh(const IMesh* pMesh) override;

        void CreateDataResources(DrawingResourceTable& resTable) override;
        virtual void BuildPass() = 0;
        std::shared_ptr<DrawingPass> GetPass(std::shared_ptr<std::string> pName) override;

        void UpdateDepthAsTexture(DrawingResourceTable& resTable);
        void UpdateShadowMapAsTarget(DrawingResourceTable& resTable);
        void UpdateShadowMapAsTexture(DrawingResourceTable& resTable);
        void UpdateScreenSpaceShadowAsTarget(DrawingResourceTable& resTable);
        void UpdateScreenSpaceShadowAsTexture(DrawingResourceTable& resTable);
        void UpdateSSAOTextureAsTarget(DrawingResourceTable& resTable);
        void UpdateSSAOTextureAsTexture(DrawingResourceTable& resTable);
        void UpdateRectTexture(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);

        void UpdateBaseColorTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture);
        void UpdateOcclusionTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture);
        void UpdateMetallicRoughnessTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture);
        void UpdateNormalTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture);
        void UpdateEmissiveTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture);

    private:
        virtual void BeginDrawPass() = 0;
        virtual void EndDrawPass() = 0;

        virtual void FlushData() = 0;
        virtual void ResetData() = 0;

        void DefineShadowCasterBlendState(DrawingResourceTable& resTable);
        void DefineShadowMapSampler(DrawingResourceTable& resTable);

        void CreateDepthTextureTarget();
        void CreateShadowmapTextureTarget();
        void CreateScreenSpaceShadowTextureTarget();
        void CreateSSAOTextureTarget();

        void UpdatePrimitive(DrawingResourceTable& resTable);
        void UpdateRectPrimitive(DrawingResourceTable& resTable);

        float4x4 UpdateWorldMatrix(const TransformComponent* pTransform);

    public:
        // Define shader resource names
        FuncResourceName(BasicVertexShader)
        FuncResourceName(BasicPixelShader)
        FuncResourceName(ScreenSpaceShadowVertexShader)
        FuncResourceName(ScreenSpaceShadowPixelShader)
        FuncResourceName(RectVertexShader)
        FuncResourceName(RectPixelShader)
        FuncResourceName(SSAOPixelShader)
        // Define effect resource names
        FuncResourceName(BasicEffect)
        FuncResourceName(ScreenSpaceShadowEffect)
        FuncResourceName(RectEffect)
        FuncResourceName(SSAOEffect)
        // Define pass names
        FuncResourceName(ShadowCasterPass)
        FuncResourceName(ScreenSpaceShadowPass)
        FuncResourceName(RectPass)
        FuncResourceName(SSAOPass)
        FuncResourceName(DebugLayerPass)

        // Vertex format names
        FuncResourceName(VertexFormatP)
        FuncResourceName(VertexFormatPN)
        FuncResourceName(VertexFormatPNT)
        FuncResourceName(VertexFormatT)
        // Vertex buffer names
        FuncResourceName(DefaultStaticPositionBuffer)
        FuncResourceName(DefaultStaticNormalBuffer)
        FuncResourceName(DefaultStaticTexcoordBuffer)
        FuncResourceName(DefaultDynamicPositionBuffer)
        FuncResourceName(DefaultDynamicNormalBuffer)
        FuncResourceName(DefaultDynamicTexcoordBuffer)
        // Index buffer names
        FuncResourceName(DefaultStaticIndexBuffer)
        FuncResourceName(DefaultDynamicIndexBuffer)
        // Constant buffer names
        FuncResourceName(DefaultWorldMatrix)
        FuncResourceName(DefaultViewMatrix)
        FuncResourceName(DefaultProjectionMatrix)
        FuncResourceName(CameraDirVector)
        FuncResourceName(LightDirVector)
        FuncResourceName(LightViewMatrix)
        FuncResourceName(LightProjMatrix)
        // Render target names
        FuncResourceName(ShadowMapTarget)
        FuncResourceName(ScreenSpaceShadowTarget)
        FuncResourceName(SSAOTarget)
        FuncResourceName(ScreenTarget)
        FuncResourceName(ScreenDepthBuffer)
        FuncResourceName(DebugLayerTarget)
        // Render state names
        FuncResourceName(DefaultDepthState)
        FuncResourceName(DepthStateNoWrite)
        FuncResourceName(DepthStateDisable)
        FuncResourceName(DefaultBlendState)
        FuncResourceName(DefaultRasterState)
        FuncResourceName(RasterStateFrontCull)
        FuncResourceName(ShadowCasterBlendState)
        // Varing states names
        FuncResourceName(DefaultVaringStates)
        // Define texture names
        FuncResourceName(ScreenDepthTexture)
        FuncResourceName(ShadowMapTexture)
        FuncResourceName(ScreenSpaceShadowTexture)
        FuncResourceName(BaseColorTexture)
        FuncResourceName(OcclusionTexture)
        FuncResourceName(MetallicRoughnessTexture)
        FuncResourceName(NormalTexture)
        FuncResourceName(EmissiveTexture)
        FuncResourceName(SSAOTexture)
        FuncResourceName(RectTexture)
        // Define texture sampler names
        FuncResourceName(ShadowMapSampler)
        FuncResourceName(LinearSampler)
        // Primitive names
        FuncResourceName(DefaultPrimitive)
        FuncResourceName(RectPrimitive)

    protected:
        std::shared_ptr<DrawingPass> CreatePass(std::shared_ptr<std::string> pName);

        std::shared_ptr<DrawingPass> CreateShadowCasterPass();
        std::shared_ptr<DrawingPass> CreateScreenSpaceShadowPass();
        std::shared_ptr<DrawingPass> CreateSSAOPass();
        std::shared_ptr<DrawingPass> CreateRectPass(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pTarget);
        std::shared_ptr<DrawingPass> CreateDebugLayerPass();

        std::shared_ptr<DrawingTransientTexture> CreateTransientTexture(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);
        std::shared_ptr<DrawingPersistTexture> CreatePersistTexture(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);

        std::shared_ptr<DrawingTransientVertexBuffer> CreateTransientVertexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);
        std::shared_ptr<DrawingPersistVertexBuffer> CreatePersistVertexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);

        std::shared_ptr<DrawingTransientIndexBuffer> CreateTransientIndexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);
        std::shared_ptr<DrawingPersistIndexBuffer> CreatePersistIndexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName);

        void DefineDefaultResources(DrawingResourceTable& resTable);

        void DefineGeneralEffect(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pSourceName, std::shared_ptr<std::string> pTechName, DrawingResourceTable& resTable);
        void DefineLinkedEffect(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pVSName, std::shared_ptr<std::string> pPSName, DrawingResourceTable& resTable);

        void DefineVertexShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable);
        void DefinePixelShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable);
        void DefineVertexShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable);
        void DefinePixelShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable);

        void DefineVertexFormatP(DrawingResourceTable& resTable);
        void DefineVertexFormatPN(DrawingResourceTable& resTable);
        void DefineVertexFormatPNT(DrawingResourceTable& resTable);
        void DefineStaticVertexBuffer(std::shared_ptr<std::string> pName, uint32_t stride, uint32_t count, const void* data, uint32_t size, DrawingResourceTable& resTable);
        void DefineStaticIndexBuffer(std::shared_ptr<std::string> pName, uint32_t count, const void* data, uint32_t size, DrawingResourceTable& resTable);

        void DefineDynamicVertexBuffer(std::shared_ptr<std::string> pName, uint32_t stride, uint32_t count, DrawingResourceTable& resTable);
        void DefineDynamicIndexBuffer(std::shared_ptr<std::string> pName, uint32_t count, DrawingResourceTable& resTable);

        void DefineWorldMatrixConstantBuffer(DrawingResourceTable& resTable);
        void DefineViewMatrixConstantBuffer(DrawingResourceTable& resTable);
        void DefineProjectionMatrixConstantBuffer(DrawingResourceTable& resTable);

        void DefineCameraDirVectorConstantBuffer(DrawingResourceTable& resTable);
        void DefineLightDirVectorConstantBuffer(DrawingResourceTable& resTable);
        void DefineLightViewMatrixConstantBuffer(DrawingResourceTable& resTable);
        void DefineLightProjMatrixConstantBuffer(DrawingResourceTable& resTable);

        void DefineDefaultDepthState(DrawingResourceTable& resTable);
        void DefineDefaultBlendState(DrawingResourceTable& resTable);
        void DefineDefaultRasterState(DrawingResourceTable& resTable);

        void DefineTarget(std::shared_ptr<std::string> pName, uint32_t width, uint32_t height, DrawingResourceTable& resTable);
        void DefineTarget(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);
        void DefineDepthBuffer(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);

        void DefineExternalTarget(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);
        void DefineExternalDepthBuffer(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);
        void DefineExternalTexture(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);

        bool DefineDynamicTexture(std::shared_ptr<std::string> pName, EDrawingFormatType format, uint32_t elementCount, DrawingResourceTable& resTable);
        void DefineDynamicTextureWithInit(std::shared_ptr<std::string> pName, EDrawingFormatType format, uint32_t elementCount, void* pData, uint32_t size, DrawingResourceTable& resTable);
        void DefineVaringStates(DrawingResourceTable& resTable);
        void DefinePrimitive(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);
        void DefineLinearSampler(DrawingResourceTable& resTable);

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

        void AddConstantSlot(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void AddTextureSlot(DrawingPass& pass, std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pParamName);

        void BindStaticInputsP(DrawingPass& pass);
        void BindDynamicInputsP(DrawingPass& pass);
        void BindStaticInputsPN(DrawingPass& pass);
        void BindDynamicInputsPN(DrawingPass& pass);
        void BindStaticInputsPNT(DrawingPass& pass);
        void BindDynamicInputsPNT(DrawingPass& pass);
        void BindStaticInputsT(DrawingPass& pass);
        void BindDynamicInputsT(DrawingPass& pass);
        void BindStates(DrawingPass& pass);
        void BindOutput(DrawingPass& pass);

        void BindConstants(DrawingPass& pass);
        void BindCameraConstants(DrawingPass& pass);
        void BindLightConstants(DrawingPass& pass);

        void BindShadowMapTexture(DrawingPass& pass);
        void BindScreenSpaceShadowTexture(DrawingPass& pass);
        void BindBaseColorTexture(DrawingPass& pass);
        void BindOcclusionTexture(DrawingPass& pass);
        void BindMetallicRoughnessTexture(DrawingPass& pass);
        void BindNormalTexture(DrawingPass& pass);
        void BindEmissiveTexture(DrawingPass& pass);
        void BindDepthTexture(DrawingPass& pass);
        void BindRectTexture(DrawingPass& pass);
        void BindLinearSampler(DrawingPass& pass);

    private:
        void DefineShaderResource(DrawingResourceTable& resTable);

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
        static const uint32_t TexcoordOffset = sizeof(float2);

        std::shared_ptr<DrawingTransientVertexBuffer> m_pTransientPositionBuffer;
        std::shared_ptr<DrawingTransientVertexBuffer> m_pTransientNormalBuffer;
        std::shared_ptr<DrawingTransientVertexBuffer> m_pTransientTexcoordBuffer;
        std::shared_ptr<DrawingTransientIndexBuffer> m_pTransientIndexBuffer;

        std::shared_ptr<DrawingTextureDepthBuffer> m_pDepthBuffer;
        std::shared_ptr<DrawingTextureTarget> m_pShadowMap;
        std::shared_ptr<DrawingTextureTarget> m_pScreenSpaceShadow;
        std::shared_ptr<DrawingTextureTarget> m_pSSAOTexture;

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