#pragma once

#include <memory>
#include <string>

#include "IRenderer.h"
#include "DrawingStage.h"
#include "DrawingPass.h"

namespace Engine
{
    class BaseRenderer : public IRenderer
    {
    public:
        BaseRenderer();
        virtual ~BaseRenderer() {}

        virtual void Initialize() override = 0;
        virtual void Shutdown() override = 0;

        virtual void Tick() override = 0;

        virtual void DefineResources(DrawingResourceTable& resTable) override = 0;
        virtual void SetupStages() override = 0;
        virtual void SetupBuffers(DrawingResourceTable& resTable) override = 0;
        virtual void Cleanup() override = 0;

        virtual void BeginFrame() override = 0;
        virtual void EndFrame() override = 0;

        virtual void UpdatePrimitive(DrawingResourceTable& resTable) override = 0;
        virtual void Draw(DrawingResourceTable& resTable) override = 0;

        void AttachDevice(const std::shared_ptr<DrawingDevice>& pDevice, const std::shared_ptr<DrawingContext>& pContext) override;
        void AttachMesh(std::shared_ptr<IMesh> pMesh) override;

        void MapResources(DrawingResourceTable& resTable) override;

    public:
        FuncResourceName(DefaultVertexFormat);

        FuncResourceName(DefaultPositionBuffer);
        FuncResourceName(DefaultNormalBuffer);

        FuncResourceName(PerVertexIndexBuffer);

        FuncResourceName(ScreenTarget);
        FuncResourceName(ScreenDepthBuffer);

        FuncResourceName(DefaultDepthState);
        FuncResourceName(DefaultBlendState);
        FuncResourceName(DefaultRasterState);

        FuncResourceName(DefaultVaringStates);

        FuncResourceName(DefaultPrimitive);

        FuncResourceName(DefaultCommandList);

    protected:
        static const uint32_t MAX_VERTEX_COUNT = 65536 * 4;
        static const uint32_t MAX_INDEX_COUNT = 65536 * 4;

        static const uint32_t PositionOffset = sizeof(Vec3<float>);
        static const uint32_t NormalOffset = sizeof(Vec3<float>);

        static char* m_sVertexID[Attribute::ESemanticType::Count][MAX_VERTEX_COUNT];
        static char* m_sIndexID[MAX_INDEX_COUNT];

        static void InitVertexID();
        static void InitInstanceID();

        uint32_t m_vertexOffset[Attribute::ESemanticType::Count];
        uint32_t m_indexOffset;

        uint32_t m_vertexCount;
        uint32_t m_indexCount;

        std::shared_ptr<DrawingDevice> m_pDevice;
        std::shared_ptr<DrawingContext> m_pDeviceContext;

        DrawingStageTable m_stageTable;

        void DefineDefaultResources(DrawingResourceTable& resTable);

        void DefineGeneralEffect(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pSourceName, std::shared_ptr<std::string> pTechName, DrawingResourceTable& resTable);
        void DefineLinkedEffect(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pVSName, std::shared_ptr<std::string> pPSName, DrawingResourceTable& resTable);

        void DefinePipelineState(std::shared_ptr<std::string> pVertexFormatName,
                                 std::shared_ptr<std::string> pPipelineStateName,
                                 std::shared_ptr<std::string> pPrimitiveName,
                                 std::shared_ptr<std::string> pVSName,
                                 std::shared_ptr<std::string> pPSName,
                                 std::shared_ptr<std::string> pBlendStateName,
                                 std::shared_ptr<std::string> pRasterStateName,
                                 std::shared_ptr<std::string> pDepthStencilStateName,
                                 std::shared_ptr<std::string> pRenderTargetName,
                                 DrawingResourceTable& resTable);

        void DefineVertexShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable);
        void DefinePixelShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable);
        void DefineVertexShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable);
        void DefinePixelShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable);
 
        void DefineDefaultVertexFormat(DrawingResourceTable& resTable);
        void DefineStaticVertexBuffer(std::shared_ptr<std::string> pName, uint32_t stride, uint32_t count, const void* data, uint32_t size, DrawingResourceTable& resTable);
        void DefineStaticIndexBuffer(std::shared_ptr<std::string> pName, uint32_t count, const void* data, uint32_t size, DrawingResourceTable& resTable);

        void DefineDefaultDepthState(DrawingResourceTable& resTable);
        void DefineDefaultBlendState(DrawingResourceTable& resTable);
        void DefineDefaultRasterState(DrawingResourceTable& resTable);

        void DefineExternalTarget(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);
        void DefineExternalDepthBuffer(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable);

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

        void BindInputs(DrawingPass& pass);
        void BindStates(DrawingPass& pass);
        void BindOutput(DrawingPass& pass);

        std::shared_ptr<DrawingStage> CreateStage(std::shared_ptr<std::string> pName);
        std::shared_ptr<DrawingPass> CreatePass(std::shared_ptr<std::string> pName);

        void FlushStage(std::shared_ptr<std::string> pStageName);

    private:
        template<typename T>
        void DoDefineShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable);

        template<typename T>
        void DoDefineShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable);
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