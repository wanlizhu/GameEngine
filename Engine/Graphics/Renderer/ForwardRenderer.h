#pragma once

#include "BaseRenderer.h"
#include "DrawingTextureTarget.h"

namespace Engine
{
    class ForwardRenderer final : public BaseRenderer
    {
    public:
        ForwardRenderer();
        virtual ~ForwardRenderer() {}

        void DefineResources(DrawingResourceTable& resTable) override;
        void SetupBuffers(DrawingResourceTable& resTable) override;
        void BuildPass() override;
        void UpdateShadowMapAsTarget(DrawingResourceTable& resTable);
        void UpdateShadowMapAsTexture(DrawingResourceTable& resTable);

    private:
        void BeginDrawPass() override;
        void EndDrawPass() override;

        void FlushData() override;
        void ResetData() override;
        void UpdatePrimitive(DrawingResourceTable& resTable) override;
        void CreateShadowmapTextureTarget();

        void DefineShadowCasterBlendState(DrawingResourceTable& resTable);
        void DefineLightDirVectorConstantBuffer(DrawingResourceTable& resTable);
        void DefineLightViewMatrixConstantBuffer(DrawingResourceTable& resTable);
        void DefineLightProjMatrixConstantBuffer(DrawingResourceTable& resTable);
        void DefineShaderResource(DrawingResourceTable& resTable);
        void DefinePipelineStateResource(DrawingResourceTable& resTable);
        void DefineShadowMapSampler(DrawingResourceTable& resTable);

        void BindLightConstants(DrawingPass& pass);
        void BindShadowMapTexture(DrawingPass& pass);

    public:
        // Define shader resource names
        FuncResourceName(ShadowCasterVertexShader)
        FuncResourceName(ShadowCasterPixelShader)
        FuncResourceName(ForwardBaseVertexShader)
        FuncResourceName(ForwardBasePixelShader)
        // Define pipeline state names
        FuncResourceName(ShadowCasterPipelineState)
        FuncResourceName(ForwardBasePipelineState)
        // Define effect resource names
        FuncResourceName(ShadowCasterEffect)
        FuncResourceName(ForwardBaseEffect)
        // Define pass names
        FuncResourceName(ShadowCasterPass)
        FuncResourceName(ForwardBasePass)
        FuncResourceName(CopyPass)
        // Constant buffer names
        FuncResourceName(LightDirVector)
        FuncResourceName(LightViewMatrix)
        FuncResourceName(LightProjMatrix)
        // Define state names
        FuncResourceName(ShadowCasterBlendState)
        // Define texture names
        FuncResourceName(ShadowMapTexture)
        // Define texture sampler names
        FuncResourceName(ShadowMapSampler)

    private:
        std::shared_ptr<DrawingPass> CreateShadowCasterPass();
        std::shared_ptr<DrawingPass> CreateForwardBasePass();
        std::shared_ptr<DrawingPass> CreateCopyPass();

    protected:
        std::shared_ptr<DrawingTextureTarget> m_pShadowMap;
    };
}