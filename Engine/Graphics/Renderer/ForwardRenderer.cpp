#include <limits>

#include "ForwardRenderer.h"
#include "DrawingDevice.h"

using namespace Engine;

ForwardRenderer::ForwardRenderer() : BaseRenderer()
{
}

void ForwardRenderer::DefineResources(DrawingResourceTable& resTable)
{
    CreateShadowmapTextureTarget();
    CreateScreenSpaceShadowTextureTarget();

    DefineDefaultResources(resTable);
    DefineShaderResource(resTable);
    DefinePipelineStateResource(resTable);
    DefineShadowCasterBlendState(resTable);

    DefineCameraDirVectorConstantBuffer(resTable);
    DefineLightDirVectorConstantBuffer(resTable);
    DefineLightViewMatrixConstantBuffer(resTable);
    DefineLightProjMatrixConstantBuffer(resTable);

    DefineExternalTexture(ShadowMapTexture(), resTable);
    DefineExternalTexture(ScreenSpaceShadowTexture(), resTable);
    DefineShadowMapSampler(resTable);
    DefinePointSampler(resTable);
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

    auto& pForwardShadingPass = CreateForwardShadingPass();
    m_passTable[ForwardShadingPass()] = pForwardShadingPass;
}

void ForwardRenderer::UpdateShadowMapAsTarget(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ShadowMapTarget());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pShadowMap->GetTarget());
}

void ForwardRenderer::UpdateShadowMapAsTexture(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ShadowMapTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pShadowMap->GetTexture());
}

void ForwardRenderer::UpdateScreenSpaceShadowAsTarget(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ScreenSpaceShadowTarget());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pScreenSpaceShadow->GetTarget());
}

void ForwardRenderer::UpdateScreenSpaceShadowAsTexture(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ScreenSpaceShadowTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pScreenSpaceShadow->GetTexture());
}

void ForwardRenderer::BeginDrawPass()
{
    m_pTransientPositionBuffer->Open();
    m_pTransientNormalBuffer->Open();
    m_pTransientIndexBuffer->Open();
}

void ForwardRenderer::EndDrawPass()
{
    m_pTransientPositionBuffer->Close();
    m_pTransientNormalBuffer->Close();
    m_pTransientIndexBuffer->Close();
}

void ForwardRenderer::FlushData()
{
    m_pTransientPositionBuffer->FlushData();
    m_pTransientNormalBuffer->FlushData();
    m_pTransientIndexBuffer->FlushData();
}

void ForwardRenderer::ResetData()
{
    m_pTransientPositionBuffer->ResetData();
    m_pTransientNormalBuffer->ResetData();
    m_pTransientIndexBuffer->ResetData();
}

void ForwardRenderer::UpdatePrimitive(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(DefaultPrimitive());
    if (pEntry == nullptr)
        return;

    auto pPrimitive = std::dynamic_pointer_cast<DrawingPrimitive>(pEntry->GetResource());
    if (pPrimitive == nullptr)
        return;

    pPrimitive->SetPrimitiveType(ePrimitive_TriangleList);
    pPrimitive->SetVertexCount(m_pTransientPositionBuffer->GetFlushOffset());
    pPrimitive->SetIndexCount(m_pTransientIndexBuffer->GetFlushOffset());
    pPrimitive->SetInstanceCount(0);

    pPrimitive->SetVertexOffset(0);
    pPrimitive->SetIndexOffset(0);
    pPrimitive->SetInstanceOffset(0);
}

void ForwardRenderer::CreateShadowmapTextureTarget()
{
    m_pShadowMap = std::make_shared<DrawingTextureTarget>(m_pDevice);
    m_pShadowMap->Initialize(gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), gpGlobal->GetConfiguration<AppConfiguration>().GetHeight(), eFormat_R32_FLOAT);
}

void ForwardRenderer::CreateScreenSpaceShadowTextureTarget()
{
    m_pScreenSpaceShadow = std::make_shared<DrawingTextureTarget>(m_pDevice);
    m_pScreenSpaceShadow->Initialize(gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), gpGlobal->GetConfiguration<AppConfiguration>().GetHeight(), eFormat_R8G8B8A8_UNORM);
}

void ForwardRenderer::DefineShadowCasterBlendState(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingBlendStateDesc>();

    pDesc->mAlphaToCoverageEnable = false;
    pDesc->mIndependentBlendEnable = false;

    for (uint32_t i = 0; i < MAX_TARGETS; ++i)
    {
        pDesc->mTargets[i].mBlendEnable = true;

        pDesc->mTargets[i].mColorBlend.mBlendSrc = eBlend_One;
        pDesc->mTargets[i].mColorBlend.mBlendDst = eBlend_Zero;
        pDesc->mTargets[i].mColorBlend.mBlendOp = eBlendOp_Min;

        pDesc->mTargets[i].mAlphaBlend.mBlendSrc = eBlend_One;
        pDesc->mTargets[i].mAlphaBlend.mBlendDst = eBlend_Zero;
        pDesc->mTargets[i].mAlphaBlend.mBlendOp = eBlendOp_Min;

        pDesc->mTargets[i].mRenderTargetWriteMask = DrawingBlendStateDesc::BlendTarget::WriteMast_All;
    }

    resTable.AddResourceEntry(ShadowCasterBlendState(), pDesc);
}

void ForwardRenderer::DefineCameraDirVectorConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gCameraDir");
    param.mType = EParam_Float3;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(CameraDirVector(), pDesc);
}

void ForwardRenderer::DefineLightDirVectorConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gLightDir");
    param.mType = EParam_Float3;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(LightDirVector(), pDesc);
}

void ForwardRenderer::DefineLightViewMatrixConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gLightViewMatrix");
    param.mType = EParam_Float4x4;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(LightViewMatrix(), pDesc);
}

void ForwardRenderer::DefineLightProjMatrixConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gLightProjMatrix");
    param.mType = EParam_Float4x4;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(LightProjMatrix(), pDesc);
}

void ForwardRenderer::DefineShaderResource(DrawingResourceTable& resTable)
{
    DefineVertexShader(BasicVertexShader(), strPtr("Asset\\Shader\\HLSL\\basic.vs"), strPtr("Basic_VS"), resTable);
    DefinePixelShader(BasicPixelShader(), strPtr("Asset\\Shader\\HLSL\\basic.ps"), strPtr("Basic_PS"), resTable);
    DefineLinkedEffect(BasicEffect(), BasicVertexShader(), BasicPixelShader(), resTable);

    DefineVertexShader(ScreenSpaceShadowVertexShader(), strPtr("Asset\\Shader\\HLSL\\screen_space_shadow.vs"), strPtr("ScreenSpaceShadow_VS"), resTable);
    DefinePixelShader(ScreenSpaceShadowPixelShader(), strPtr("Asset\\Shader\\HLSL\\screen_space_shadow.ps"), strPtr("ScreenSpaceShadow_PS"), resTable);
    DefineLinkedEffect(ScreenSpaceShadowEffect(), ScreenSpaceShadowVertexShader(), ScreenSpaceShadowPixelShader(), resTable);

    DefineVertexShader(ForwardShadingVertexShader(), strPtr("Asset\\Shader\\HLSL\\forward_shading.vs"), strPtr("ForwardShading_VS"), resTable);
    DefinePixelShader(ForwardShadingPixelShader(), strPtr("Asset\\Shader\\HLSL\\forward_shading.ps"), strPtr("ForwardShading_PS"), resTable);
    DefineLinkedEffect(ForwardShadingEffect(), ForwardShadingVertexShader(), ForwardShadingPixelShader(), resTable);
}

void ForwardRenderer::DefinePipelineStateResource(DrawingResourceTable& resTable)
{
    DefinePipelineState(DepthPipelineState(),
                        VertexFormatP(),
                        DefaultPrimitive(),
                        BasicEffect(),
                        DefaultBlendState(),
                        DefaultRasterState(),
                        DefaultDepthState(),
                        nullptr,
                        resTable);

    DefinePipelineState(ShadowCasterPipelineState(),
                        VertexFormatP(),
                        DefaultPrimitive(),
                        BasicEffect(),
                        ShadowCasterBlendState(),
                        DefaultRasterState(),
                        DepthStateDisable(),
                        ShadowMapTarget(),
                        resTable);

    DefinePipelineState(ScreenSpaceShadowPipelineState(),
                        VertexFormatPN(),
                        DefaultPrimitive(),
                        ScreenSpaceShadowEffect(),
                        DefaultBlendState(),
                        DefaultRasterState(),
                        DepthStateNoWrite(),
                        ScreenSpaceShadowTarget(),
                        resTable);

    DefinePipelineState(ForwardShadingPipelineState(),
                        VertexFormatPN(),
                        DefaultPrimitive(),
                        ForwardShadingEffect(),
                        DefaultBlendState(),
                        DefaultRasterState(),
                        DepthStateNoWrite(),
                        ScreenTarget(),
                        resTable);
}

void ForwardRenderer::DefineShadowMapSampler(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingSamplerStateDesc>();

    pDesc->mSamplerMode = eSamplerMode_Compare;
    pDesc->mAddressU = eAddressMode_Clamp;
    pDesc->mAddressV = eAddressMode_Clamp;
    pDesc->mAddressW = eAddressMode_Clamp;
    pDesc->mBorderColor[0] = 0;
    pDesc->mBorderColor[1] = 0;
    pDesc->mBorderColor[2] = 0;
    pDesc->mBorderColor[3] = 0;
    pDesc->mComparisonFunc = eComparison_Always;
    pDesc->mMinFilter = eFilterMode_Linear;
    pDesc->mMagFilter = eFilterMode_Linear;
    pDesc->mMipFilter = eFilterMode_Linear;
    pDesc->mMinLOD = 0;
    pDesc->mMaxLOD = std::numeric_limits<float>::max();
    pDesc->mMipLODBias = 0.0f;
    pDesc->mMaxAnisotropy = 1;

    resTable.AddResourceEntry(ShadowMapSampler(), pDesc);
}

void ForwardRenderer::DefinePointSampler(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingSamplerStateDesc>();

    pDesc->mSamplerMode = eSamplerMode_Compare;
    pDesc->mAddressU = eAddressMode_Border;
    pDesc->mAddressV = eAddressMode_Border;
    pDesc->mAddressW = eAddressMode_Border;
    pDesc->mBorderColor[0] = 0;
    pDesc->mBorderColor[1] = 0;
    pDesc->mBorderColor[2] = 0;
    pDesc->mBorderColor[3] = 0;
    pDesc->mComparisonFunc = eComparison_Always;
    pDesc->mMinFilter = eFilterMode_Point;
    pDesc->mMagFilter = eFilterMode_Point;
    pDesc->mMipFilter = eFilterMode_Point;
    pDesc->mMinLOD = 0;
    pDesc->mMaxLOD = std::numeric_limits<float>::max();
    pDesc->mMipLODBias = 0.0f;
    pDesc->mMaxAnisotropy = 1;

    resTable.AddResourceEntry(PointSampler(), pDesc);
}

void ForwardRenderer::BindCameraConstants(DrawingPass& pass)
{
    AddConstantSlot(pass, CameraDirVector());
}

void ForwardRenderer::BindLightConstants(DrawingPass& pass)
{
    AddConstantSlot(pass, LightDirVector());
    AddConstantSlot(pass, LightViewMatrix());
    AddConstantSlot(pass, LightProjMatrix());
}

void ForwardRenderer::BindShadowMapTexture(DrawingPass& pass)
{
    auto shadowmap_tex_slot = strPtr("ShadowMapTex");
    AddTextureSlot(pass, shadowmap_tex_slot, strPtr("gShadowMapTexture"));
    BindResource(pass, shadowmap_tex_slot, ShadowMapTexture());

    auto shadowmap_sampler_slot = strPtr("ShadowMapSampler");
    pass.AddResourceSlot(shadowmap_sampler_slot, ResourceSlot_Sampler, strPtr("gShadowMapSampler"));
    BindResource(pass, shadowmap_sampler_slot, ShadowMapSampler());
}

void ForwardRenderer::BindScreenSpaceShadowTexture(DrawingPass& pass)
{
    auto screenspaceshadow_tex_slot = strPtr("ScreenSpaceShadowTex");
    AddTextureSlot(pass, screenspaceshadow_tex_slot, strPtr("gScreenSpaceShadowTexture"));
    BindResource(pass, screenspaceshadow_tex_slot, ScreenSpaceShadowTexture());

    auto point_sampler_slot = strPtr("PointSampler");
    pass.AddResourceSlot(point_sampler_slot, ResourceSlot_Sampler, strPtr("gPointSampler"));
    BindResource(pass, point_sampler_slot, PointSampler());
}

std::shared_ptr<DrawingPass> ForwardRenderer::CreateDepthPass()
{
    auto pPass = CreatePass(DepthPass());

    BindEffect(*pPass, BasicEffect());
    BindPipelineState(*pPass, DepthPipelineState());
    BindDynamicInputsP(*pPass);
    BindStates(*pPass);
    BindDepthBuffer(*pPass, ScreenDepthBuffer());
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());

    BindConstants(*pPass);

    return pPass;
}

std::shared_ptr<DrawingPass> ForwardRenderer::CreateShadowCasterPass()
{
    auto pPass = CreatePass(ShadowCasterPass());

    BindEffect(*pPass, BasicEffect());
    BindPipelineState(*pPass, ShadowCasterPipelineState());
    BindDynamicInputsP(*pPass);
    BindDepthState(*pPass, DepthStateDisable());
    BindBlendState(*pPass, ShadowCasterBlendState());
    BindRasterState(*pPass, DefaultRasterState());
    BindTarget(*pPass, 0, ShadowMapTarget());
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());

    BindConstants(*pPass);

    return pPass;
}

std::shared_ptr<DrawingPass> ForwardRenderer::CreateScreenSpaceShadowPass()
{
    auto pPass = CreatePass(ScreenSpaceShadowPass());

    BindEffect(*pPass, ScreenSpaceShadowEffect());
    BindPipelineState(*pPass, ScreenSpaceShadowPipelineState());
    BindDynamicInputsPN(*pPass);
    BindDepthState(*pPass, DepthStateNoWrite());
    BindBlendState(*pPass, DefaultBlendState());
    BindRasterState(*pPass, DefaultRasterState());
    BindTarget(*pPass, 0, ScreenSpaceShadowTarget());
    BindDepthBuffer(*pPass, ScreenDepthBuffer());
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());
    BindShadowMapTexture(*pPass);

    BindConstants(*pPass);
    BindLightConstants(*pPass);

    return pPass;
}

std::shared_ptr<DrawingPass> ForwardRenderer::CreateForwardShadingPass()
{
    auto pPass = CreatePass(ForwardShadingPass());

    BindEffect(*pPass, ForwardShadingEffect());
    BindPipelineState(*pPass, ForwardShadingPipelineState());
    BindDynamicInputsPN(*pPass);
    BindDepthState(*pPass, DepthStateNoWrite());
    BindBlendState(*pPass, DefaultBlendState());
    BindRasterState(*pPass, DefaultRasterState());
    BindOutput(*pPass);
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());
    BindScreenSpaceShadowTexture(*pPass);

    BindConstants(*pPass);
    BindLightConstants(*pPass);
    BindCameraConstants(*pPass);

    return pPass;
}