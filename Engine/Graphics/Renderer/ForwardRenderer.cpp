#include <limits>

#include "ForwardRenderer.h"
#include "DrawingDevice.h"

using namespace Engine;

ForwardRenderer::ForwardRenderer() : BaseRenderer()
{
}

void ForwardRenderer::DefineResources(DrawingResourceTable& resTable)
{
    DefineDefaultResources(resTable);
    DefineShaderResource(resTable);

    DefineCameraDirVectorConstantBuffer(resTable);
    DefineLightDirVectorConstantBuffer(resTable);
    DefineLightViewMatrixConstantBuffer(resTable);
    DefineLightProjMatrixConstantBuffer(resTable);
}

void ForwardRenderer::SetupBuffers(DrawingResourceTable& resTable)
{
}

void ForwardRenderer::BuildPass()
{
    auto& pDepthPass = CreateDepthPass();
    m_passTable[DepthPass()] = pDepthPass;

    auto& pShadowCasterPass = CreateShadowCasterPass();
    m_passTable[ShadowCasterPass()] = pShadowCasterPass;

    auto& pSSSPass = CreateScreenSpaceShadowPass();
    m_passTable[ScreenSpaceShadowPass()] = pSSSPass;

    auto& pSSAOPass = CreateSSAOPass();
    m_passTable[SSAOPass()] = pSSAOPass;

    auto& pForwardShadingPass = CreateForwardShadingPass();
    m_passTable[ForwardShadingPass()] = pForwardShadingPass;

    auto& pDebugLayerPass = CreateDebugLayerPass();
    m_passTable[DebugLayerPass()] = pDebugLayerPass;
}

void ForwardRenderer::BeginDrawPass()
{
    m_pTransientPositionBuffer->Open();
    m_pTransientNormalBuffer->Open();
    m_pTransientTexcoordBuffer->Open();
    m_pTransientIndexBuffer->Open();
}

void ForwardRenderer::EndDrawPass()
{
    m_pTransientPositionBuffer->Close();
    m_pTransientNormalBuffer->Close();
    m_pTransientTexcoordBuffer->Close();
    m_pTransientIndexBuffer->Close();
}

void ForwardRenderer::FlushData()
{
    m_pTransientPositionBuffer->FlushData();
    m_pTransientNormalBuffer->FlushData();
    m_pTransientTexcoordBuffer->FlushData();
    m_pTransientIndexBuffer->FlushData();
}

void ForwardRenderer::ResetData()
{
    m_pTransientPositionBuffer->ResetData();
    m_pTransientNormalBuffer->ResetData();
    m_pTransientTexcoordBuffer->ResetData();
    m_pTransientIndexBuffer->ResetData();
}

void ForwardRenderer::DefineShaderResource(DrawingResourceTable& resTable)
{
    DefineVertexShader(ForwardShadingVertexShader(), strPtr("Asset\\Shader\\HLSL\\forward_shading.vs"), strPtr("ForwardShading_VS"), resTable);
    DefinePixelShader(ForwardShadingPixelShader(), strPtr("Asset\\Shader\\HLSL\\forward_shading.ps"), strPtr("ForwardShading_PS"), resTable);
    DefineLinkedEffect(ForwardShadingEffect(), ForwardShadingVertexShader(), ForwardShadingPixelShader(), resTable);
}

std::shared_ptr<DrawingPass> ForwardRenderer::CreateDepthPass()
{
    auto pPass = CreatePass(DepthPass());

    BindEffect(*pPass, BasicEffect());
    BindDynamicInputsP(*pPass);
    BindStates(*pPass);
    BindDepthBuffer(*pPass, ScreenDepthBuffer());
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());

    BindConstants(*pPass);

    return pPass;
}

std::shared_ptr<DrawingPass> ForwardRenderer::CreateForwardShadingPass()
{
    auto pPass = CreatePass(ForwardShadingPass());

    BindEffect(*pPass, ForwardShadingEffect());
    BindDynamicInputsPNT(*pPass);
    BindDepthState(*pPass, DepthStateNoWrite());
    BindBlendState(*pPass, DefaultBlendState());
    BindRasterState(*pPass, DefaultRasterState());
    BindOutput(*pPass);
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());
    BindScreenSpaceShadowTexture(*pPass);
    BindBaseColorTexture(*pPass);
    BindOcclusionTexture(*pPass);
    BindMetallicRoughnessTexture(*pPass);
    BindNormalTexture(*pPass);
    BindEmissiveTexture(*pPass);
    BindLinearSampler(*pPass);

    BindConstants(*pPass);
    BindLightConstants(*pPass);
    BindCameraConstants(*pPass);

    return pPass;
}