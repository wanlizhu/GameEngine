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

        void UpdateScreenSpaceShadowAsTarget(DrawingResourceTable& resTable);
        void UpdateScreenSpaceShadowAsTexture(DrawingResourceTable& resTable);

    private:
        void BeginDrawPass() override;
        void EndDrawPass() override;

        void FlushData() override;
        void ResetData() override;
        void UpdatePrimitive(DrawingResourceTable& resTable) override;
        void CreateShadowmapTextureTarget();
        void CreateScreenSpaceShadowTextureTarget();

        void DefineShadowCasterBlendState(DrawingResourceTable& resTable);
        void DefineCameraDirVectorConstantBuffer(DrawingResourceTable& resTable);
        void DefineLightDirVectorConstantBuffer(DrawingResourceTable& resTable);
        void DefineLightViewMatrixConstantBuffer(DrawingResourceTable& resTable);
        void DefineLightProjMatrixConstantBuffer(DrawingResourceTable& resTable);
        void DefineShaderResource(DrawingResourceTable& resTable);
        void DefinePipelineStateResource(DrawingResourceTable& resTable);
        void DefineShadowMapSampler(DrawingResourceTable& resTable);
        void DefinePointSampler(DrawingResourceTable& resTable);

        void BindCameraConstants(DrawingPass& pass);
        void BindLightConstants(DrawingPass& pass);
        void BindShadowMapTexture(DrawingPass& pass);
        void BindScreenSpaceShadowTexture(DrawingPass& pass);

    public:
        // Define shader resource names
        FuncResourceName(BasicVertexShader)
        FuncResourceName(BasicPixelShader)
        FuncResourceName(ScreenSpaceShadowVertexShader)
        FuncResourceName(ScreenSpaceShadowPixelShader)
        FuncResourceName(ForwardShadingVertexShader)
        FuncResourceName(ForwardShadingPixelShader)
        // Define pipeline state names
        FuncResourceName(DepthPipelineState)
        FuncResourceName(ShadowCasterPipelineState)
        FuncResourceName(ScreenSpaceShadowPipelineState)
        FuncResourceName(ForwardShadingPipelineState)
        // Define effect resource names
        FuncResourceName(BasicEffect)
        FuncResourceName(ScreenSpaceShadowEffect)
        FuncResourceName(ForwardShadingEffect)
        // Define pass names
        FuncResourceName(DepthPass)
        FuncResourceName(ShadowCasterPass)
        FuncResourceName(ScreenSpaceShadowPass)
        FuncResourceName(ForwardShadingPass)
        // Constant buffer names
        FuncResourceName(CameraDirVector)
        FuncResourceName(LightDirVector)
        FuncResourceName(LightViewMatrix)
        FuncResourceName(LightProjMatrix)
        // Define state names
        FuncResourceName(ShadowCasterBlendState)
        // Define texture names
        FuncResourceName(ShadowMapTexture)
        FuncResourceName(ScreenSpaceShadowTexture)
        // Define texture sampler names
        FuncResourceName(ShadowMapSampler)
        FuncResourceName(PointSampler)

    private:
        std::shared_ptr<DrawingPass> CreateDepthPass();
        std::shared_ptr<DrawingPass> CreateShadowCasterPass();
        std::shared_ptr<DrawingPass> CreateScreenSpaceShadowPass();
        std::shared_ptr<DrawingPass> CreateForwardShadingPass();

    protected:
        std::shared_ptr<DrawingTextureTarget> m_pShadowMap;
        std::shared_ptr<DrawingTextureTarget> m_pScreenSpaceShadow;
    };
}