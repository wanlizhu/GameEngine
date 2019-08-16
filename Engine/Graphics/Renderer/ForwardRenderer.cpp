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

    DefineDefaultResources(resTable);
    DefineShaderResource(resTable);
    DefinePipelineStateResource(resTable);
    DefineShadowCasterBlendState(resTable);

    DefineLightDirVectorConstantBuffer(resTable);
    DefineLightViewMatrixConstantBuffer(resTable);
    DefineLightProjMatrixConstantBuffer(resTable);

    DefineExternalTexture(ShadowMapTexture(), resTable);
    DefineShadowMapSampler(resTable);
}

void ForwardRenderer::SetupBuffers(DrawingResourceTable& resTable)
{
}

void ForwardRenderer::BuildPass()
{
    auto& pShadowCaster = CreateShadowCasterPass();
    m_passTable[ShadowCasterPass()] = pShadowCaster;

    auto& pMainPass = CreateForwardBasePass();
    m_passTable[ForwardBasePass()] = pMainPass;

    auto& pCopyPass = CreateCopyPass();
    m_passTable[CopyPass()] = pCopyPass;
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
    DefineVertexShader(ShadowCasterVertexShader(), strPtr("Asset\\Shader\\HLSL\\shadow_caster.vs"), strPtr("ShadowCaster_VS"), resTable);
    DefinePixelShader(ShadowCasterPixelShader(), strPtr("Asset\\Shader\\HLSL\\shadow_caster.ps"), strPtr("ShadowCaster_PS"), resTable);
    DefineLinkedEffect(ShadowCasterEffect(), ShadowCasterVertexShader(), ShadowCasterPixelShader(), resTable);

    DefineVertexShader(ForwardBaseVertexShader(), strPtr("Asset\\Shader\\HLSL\\forward_base.vs"), strPtr("ForwardBase_VS"), resTable);
    DefinePixelShader(ForwardBasePixelShader(), strPtr("Asset\\Shader\\HLSL\\forward_base.ps"), strPtr("ForwardBase_PS"), resTable);
    DefineLinkedEffect(ForwardBaseEffect(), ForwardBaseVertexShader(), ForwardBasePixelShader(), resTable);
}

void ForwardRenderer::DefinePipelineStateResource(DrawingResourceTable& resTable)
{
    DefinePipelineState(ShadowCasterPipelineState(),
                        VertexFormatP(),
                        DefaultPrimitive(),
                        ShadowCasterEffect(),
                        ShadowCasterBlendState(),
                        DefaultRasterState(),
                        DefaultDepthState(),
                        ShadowMapTarget(),
                        resTable);

    DefinePipelineState(ForwardBasePipelineState(),
                        VertexFormatPN(),
                        DefaultPrimitive(),
                        ForwardBaseEffect(),
                        DefaultBlendState(),
                        DefaultRasterState(),
                        DefaultDepthState(),
                        DefaultTarget(),
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

std::shared_ptr<DrawingPass> ForwardRenderer::CreateShadowCasterPass()
{
    auto pPass = CreatePass(ShadowCasterPass());

    BindEffect(*pPass, ShadowCasterEffect());
    BindPipelineState(*pPass, ShadowCasterPipelineState());
    BindDynamicInputsP(*pPass);
    BindDepthState(*pPass, DefaultDepthState());
    BindBlendState(*pPass, ShadowCasterBlendState());
    BindRasterState(*pPass, DefaultRasterState());
    BindTarget(*pPass, 0, ShadowMapTarget());
    BindDepthBuffer(*pPass, DefaultDepthBuffer());
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());

    AddConstantSlot(*pPass, DefaultWorldMatrix());
    AddConstantSlot(*pPass, LightViewMatrix());
    AddConstantSlot(*pPass, LightProjMatrix());

    return pPass;
}

std::shared_ptr<DrawingPass> ForwardRenderer::CreateForwardBasePass()
{
    auto pPass = CreatePass(ForwardBasePass());

    BindEffect(*pPass, ForwardBaseEffect());
    BindPipelineState(*pPass, ForwardBasePipelineState());
    BindDynamicInputsPN(*pPass);
    BindStates(*pPass);
    BindOutput(*pPass);
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());
    BindShadowMapTexture(*pPass);

    BindConstants(*pPass);
    BindLightConstants(*pPass);

    return pPass;
}

std::shared_ptr<DrawingPass> ForwardRenderer::CreateCopyPass()
{
    auto pPass = CreatePass(CopyPass());
    return pPass;
}