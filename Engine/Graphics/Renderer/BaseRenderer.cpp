#include "Matrix.h"
#include "BaseRenderer.h"
#include "DrawingResourceDesc.h"

using namespace Engine;

const uint32_t DrawingLinkedEffectDesc::VERTEX_SHADER_ID;
const uint32_t DrawingLinkedEffectDesc::PIXEL_SHADER_ID;

BaseRenderer::BaseRenderer()
{
}

void BaseRenderer::CreateDataResources(DrawingResourceTable& resTable)
{
    m_pTransientPositionBuffer = CreateTransientVertexBuffer(resTable, DefaultDynamicPositionBuffer());
    m_pTransientNormalBuffer = CreateTransientVertexBuffer(resTable, DefaultDynamicNormalBuffer());
    m_pTransientTexcoordBuffer = CreateTransientVertexBuffer(resTable, DefaultDynamicTexcoordBuffer());
    m_pTransientIndexBuffer = CreateTransientIndexBuffer(resTable, DefaultDynamicIndexBuffer());
}

std::shared_ptr<DrawingPass> BaseRenderer::GetPass(std::shared_ptr<std::string> pName)
{
    auto iter = m_passTable.find(pName);
    if (iter == m_passTable.end())
        return nullptr;

    return iter->second;
}

void BaseRenderer::UpdateDepthAsTexture(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ScreenDepthTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pDepthBuffer->GetTexture());
}

void BaseRenderer::UpdateShadowMapAsTarget(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ShadowMapTarget());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pShadowMap->GetTarget());
}

void BaseRenderer::UpdateShadowMapAsTexture(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ShadowMapTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pShadowMap->GetTexture());
}

void BaseRenderer::UpdateScreenSpaceShadowAsTarget(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ScreenSpaceShadowTarget());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pScreenSpaceShadow->GetTarget());
}

void BaseRenderer::UpdateScreenSpaceShadowAsTexture(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ScreenSpaceShadowTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pScreenSpaceShadow->GetTexture());
}

void BaseRenderer::UpdateSSAOTextureAsTarget(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(SSAOTarget());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pSSAOTexture->GetTarget());
}

void BaseRenderer::UpdateSSAOTextureAsTexture(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(SSAOTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(m_pSSAOTexture->GetTexture());
}

void BaseRenderer::UpdateRectTexture(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName)
{
    auto pEntry = resTable.GetResourceEntry(RectTexture());
    auto pEntrySrc = resTable.GetResourceEntry(pName);

    assert(pEntry != nullptr);
    assert(pEntrySrc != nullptr);

    pEntry->SetExternalResource(pEntrySrc->GetResource());
}

void BaseRenderer::UpdateBaseColorTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture)
{
    auto pEntry = resTable.GetResourceEntry(BaseColorTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(pTexture);
}

void BaseRenderer::UpdateOcclusionTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture)
{
    auto pEntry = resTable.GetResourceEntry(OcclusionTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(pTexture);
}

void BaseRenderer::UpdateMetallicRoughnessTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture)
{
    auto pEntry = resTable.GetResourceEntry(MetallicRoughnessTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(pTexture);
}

void BaseRenderer::UpdateNormalTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture)
{
    auto pEntry = resTable.GetResourceEntry(NormalTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(pTexture);
}

void BaseRenderer::UpdateEmissiveTexture(DrawingResourceTable& resTable, std::shared_ptr<DrawingResource> pTexture)
{
    auto pEntry = resTable.GetResourceEntry(EmissiveTexture());
    assert(pEntry != nullptr);
    pEntry->SetExternalResource(pTexture);
}

void BaseRenderer::AddRenderables(RenderQueueItemListType renderables)
{
    m_renderQueue.Reset();
    for (auto& item : renderables)
        item.pRenderable->GetRenderable(m_renderQueue, item.pTransformComp);
}

void BaseRenderer::Clear(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass)
{
}

void BaseRenderer::Render(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass)
{
    m_renderQueue.Dispatch(ERenderQueueType::Opaque, [&](const RenderQueueItem& item) -> void {
        auto pMesh = dynamic_cast<const IMesh*>(item.pRenderable);
        if (pMesh == nullptr)
            return;

        auto trans = UpdateWorldMatrix(item.pTransformComp);
        m_pDeviceContext->UpdateTransform(resTable, trans);

        BeginDrawPass();
        AttachMesh(pMesh);
        FlushData();
        UpdatePrimitive(resTable);

        pPass->Flush(*m_pDeviceContext);

        ResetData();
        EndDrawPass();
    });
}

void BaseRenderer::RenderRect(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass)
{
    UpdateRectPrimitive(resTable);

    pPass->Flush(*m_pDeviceContext);
}

void BaseRenderer::CopyRect(DrawingResourceTable& resTable, std::shared_ptr<std::string> pSrcName, std::shared_ptr<std::string> pDstName, const int2& dstOrigin)
{
    auto pSrcEntry = resTable.GetResourceEntry(pSrcName);
    auto pDstEntry = resTable.GetResourceEntry(pDstName);

    assert(pSrcEntry != nullptr);
    assert(pDstEntry != nullptr);

    m_pDevice->CopyTexture(pDstEntry->GetResource(), pSrcEntry->GetResource(), 0, 0, int3(0, 0, 0), int3(gpGlobal->GetConfiguration<DebugConfiguration>().GetWidth(), gpGlobal->GetConfiguration<DebugConfiguration>().GetHeight(), 1), int3(dstOrigin.x, dstOrigin.y, 0));
}

void BaseRenderer::AttachDevice(const std::shared_ptr<DrawingDevice>& pDevice, const std::shared_ptr<DrawingContext>& pContext)
{
    m_pDevice = pDevice;
    m_pDeviceContext = pContext;
}

void BaseRenderer::AttachMesh(const IMesh* pMesh)
{
    auto vertexCount = pMesh->VertexCount();
    auto indexCount = pMesh->IndexCount();

    assert(m_pTransientPositionBuffer->CheckCapacity(vertexCount));
    assert(m_pTransientNormalBuffer->CheckCapacity(vertexCount));
    assert(m_pTransientTexcoordBuffer->CheckCapacity(vertexCount));
    assert(m_pTransientIndexBuffer->CheckCapacity(indexCount));

    auto pAttributes = pMesh->GetAttributes();

    std::for_each(pAttributes.cbegin(), pAttributes.cend(), [&](std::shared_ptr<Attribute> pElem)
    {
        auto type = (uint32_t)pElem->semanticType;
        
        if (type == (uint32_t)Attribute::ESemanticType::Position)
            m_pTransientPositionBuffer->FillData(pElem->pData.get(), vertexCount);

        if (type == (uint32_t)Attribute::ESemanticType::Normal)
            m_pTransientNormalBuffer->FillData(pElem->pData.get(), vertexCount);

        if (type == (uint32_t)Attribute::ESemanticType::Texcoord0)
            m_pTransientTexcoordBuffer->FillData(pElem->pData.get(), vertexCount);
    });

    m_pTransientIndexBuffer->FillData(pMesh->GetIndexData().get(), indexCount);
}

std::shared_ptr<DrawingPass> BaseRenderer::CreatePass(std::shared_ptr<std::string> pName)
{
    return std::make_shared<DrawingPass>(pName, m_pDevice);
}

std::shared_ptr<DrawingPass> BaseRenderer::CreateShadowCasterPass()
{
    auto pPass = CreatePass(ShadowCasterPass());

    BindEffect(*pPass, BasicEffect());
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

std::shared_ptr<DrawingPass> BaseRenderer::CreateScreenSpaceShadowPass()
{
    auto pPass = CreatePass(ScreenSpaceShadowPass());

    BindEffect(*pPass, ScreenSpaceShadowEffect());
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

std::shared_ptr<DrawingPass> BaseRenderer::CreateSSAOPass()
{
    auto pPass = CreatePass(SSAOPass());

    BindEffect(*pPass, SSAOEffect());
    BindStates(*pPass);
    BindTarget(*pPass, 0, SSAOTarget());
    BindPrimitive(*pPass, RectPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());
    BindDepthTexture(*pPass);
    BindLinearSampler(*pPass);

    return pPass;
}

std::shared_ptr<DrawingPass> BaseRenderer::CreateRectPass(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pTarget)
{
    auto pPass = CreatePass(pName);

    BindEffect(*pPass, pEffectName);
    BindStates(*pPass);
    BindTarget(*pPass, 0, pTarget);
    BindPrimitive(*pPass, RectPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());
    BindRectTexture(*pPass);
    BindLinearSampler(*pPass);

    return pPass;
}

 std::shared_ptr<DrawingPass> BaseRenderer::CreateDebugLayerPass()
 {
    auto pPass = CreateRectPass(DebugLayerPass(), RectEffect(), DebugLayerTarget());
    return pPass;
 }

std::shared_ptr<DrawingTransientTexture> BaseRenderer::CreateTransientTexture(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName)
{
    auto pEntry = resTable.GetResourceEntry(pName);
    assert(pEntry != nullptr);

    auto pTex = std::dynamic_pointer_cast<DrawingTexture>(pEntry->GetResource());
    return std::make_shared<DrawingTransientTexture>(pTex);
}

std::shared_ptr<DrawingPersistTexture> BaseRenderer::CreatePersistTexture(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName)
{
    auto pEntry = resTable.GetResourceEntry(pName);
    assert(pEntry != nullptr);

    auto pTex = std::dynamic_pointer_cast<DrawingTexture>(pEntry->GetResource());
    return std::make_shared<DrawingPersistTexture>(pTex);
}

std::shared_ptr<DrawingTransientVertexBuffer> BaseRenderer::CreateTransientVertexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName)
{
    auto pEntry = resTable.GetResourceEntry(pName);
    assert(pEntry != nullptr);

    auto pTex = std::dynamic_pointer_cast<DrawingVertexBuffer>(pEntry->GetResource());
    return std::make_shared<DrawingTransientVertexBuffer>(pTex);
}

std::shared_ptr<DrawingPersistVertexBuffer> BaseRenderer::CreatePersistVertexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName)
{
    auto pEntry = resTable.GetResourceEntry(pName);
    assert(pEntry != nullptr);

    auto pTex = std::dynamic_pointer_cast<DrawingVertexBuffer>(pEntry->GetResource());
    return std::make_shared<DrawingPersistVertexBuffer>(pTex);
}

std::shared_ptr<DrawingTransientIndexBuffer> BaseRenderer::CreateTransientIndexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName)
{
    auto pEntry = resTable.GetResourceEntry(pName);
    assert(pEntry != nullptr);

    auto pTex = std::dynamic_pointer_cast<DrawingIndexBuffer>(pEntry->GetResource());
    return std::make_shared<DrawingTransientIndexBuffer>(pTex);
}

std::shared_ptr<DrawingPersistIndexBuffer> BaseRenderer::CreatePersistIndexBuffer(DrawingResourceTable& resTable, std::shared_ptr<std::string> pName)
{
    auto pEntry = resTable.GetResourceEntry(pName);
    assert(pEntry != nullptr);

    auto pTex = std::dynamic_pointer_cast<DrawingIndexBuffer>(pEntry->GetResource());
    return std::make_shared<DrawingPersistIndexBuffer>(pTex);
}

void BaseRenderer::DefineDefaultResources(DrawingResourceTable& resTable)
{
    CreateDepthTextureTarget();
    CreateShadowmapTextureTarget();
    CreateScreenSpaceShadowTextureTarget();
    CreateSSAOTextureTarget();

    DefineVertexFormatP(resTable);
    DefineVertexFormatPN(resTable);
    DefineVertexFormatPNT(resTable);

    DefineDynamicVertexBuffer(DefaultDynamicPositionBuffer(), PositionOffset, MAX_VERTEX_COUNT, resTable);
    DefineDynamicVertexBuffer(DefaultDynamicNormalBuffer(), NormalOffset, MAX_VERTEX_COUNT, resTable);
    DefineDynamicVertexBuffer(DefaultDynamicTexcoordBuffer(), TexcoordOffset, MAX_VERTEX_COUNT, resTable);
    DefineDynamicIndexBuffer(DefaultDynamicIndexBuffer(), MAX_INDEX_COUNT, resTable);

    DefineWorldMatrixConstantBuffer(resTable);
    DefineViewMatrixConstantBuffer(resTable);
    DefineProjectionMatrixConstantBuffer(resTable);

    DefineTarget(DebugLayerTarget(), gpGlobal->GetConfiguration<DebugConfiguration>().GetWidth(), gpGlobal->GetConfiguration<DebugConfiguration>().GetHeight(), resTable);
    DefineShaderResource(resTable);

    DefineExternalTarget(ShadowMapTarget(), resTable);
    DefineExternalTarget(ScreenSpaceShadowTarget(), resTable);
    DefineExternalTarget(SSAOTarget(), resTable);
    DefineExternalTarget(ScreenTarget(), resTable);
    DefineExternalDepthBuffer(ScreenDepthBuffer(), resTable);

    DefineDefaultDepthState(resTable);
    DefineDefaultBlendState(resTable);
    DefineDefaultRasterState(resTable);
    DefineShadowCasterBlendState(resTable);

    DefineExternalTexture(ScreenDepthTexture(), resTable);
    DefineExternalTexture(ShadowMapTexture(), resTable);
    DefineExternalTexture(ScreenSpaceShadowTexture(), resTable);
    DefineExternalTexture(BaseColorTexture(), resTable);
    DefineExternalTexture(OcclusionTexture(), resTable);
    DefineExternalTexture(MetallicRoughnessTexture(), resTable);
    DefineExternalTexture(EmissiveTexture(), resTable);
    DefineExternalTexture(NormalTexture(), resTable);
    DefineExternalTexture(SSAOTexture(), resTable);
    DefineShadowMapSampler(resTable);

    DefineExternalTexture(RectTexture(), resTable);
    DefineLinearSampler(resTable);

    DefinePrimitive(DefaultPrimitive(), resTable);
    DefinePrimitive(RectPrimitive(), resTable);
    DefineVaringStates(resTable);
}


void BaseRenderer::DefineGeneralEffect(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pSourceName, std::shared_ptr<std::string> pTechName, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingGeneralEffectDesc>();

    pDesc->mProgramType = eProgram_Binary;
    pDesc->mpName = pEffectName;
    pDesc->mpSourceName = pSourceName;
    pDesc->mpTechName = pTechName;

    resTable.AddResourceEntry(pEffectName, pDesc);
}

void BaseRenderer::DefineLinkedEffect(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pVSName, std::shared_ptr<std::string> pPSName, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingLinkedEffectDesc>();

    pDesc->mProgramType = eProgram_Shader;
    pDesc->mpName = pEffectName;

    pDesc->AddResourceDescName(DrawingLinkedEffectDesc::VERTEX_SHADER_ID, pVSName);
    pDesc->AddResourceDescName(DrawingLinkedEffectDesc::PIXEL_SHADER_ID, pPSName);

    resTable.AddResourceEntry(pEffectName, pDesc);
}

void BaseRenderer::DefineVertexShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable)
{
    DoDefineShaderFromBlob<DrawingVertexShaderDesc>(pShaderName, pSourceName, resTable);
}

void BaseRenderer::DefinePixelShaderFromBlob(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pSourceName, DrawingResourceTable& resTable)
{
    DoDefineShaderFromBlob<DrawingPixelShaderDesc>(pShaderName, pSourceName, resTable);
}

void BaseRenderer::DefineVertexShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable)
{
    DoDefineShader<DrawingVertexShaderDesc>(pShaderName, pFileName, pEntryName, resTable);
}

void BaseRenderer::DefinePixelShader(std::shared_ptr<std::string> pShaderName, std::shared_ptr<std::string> pFileName, std::shared_ptr<std::string> pEntryName, DrawingResourceTable& resTable)
{
    DoDefineShader<DrawingPixelShaderDesc>(pShaderName, pFileName, pEntryName, resTable);
}

void BaseRenderer::DefineVertexFormatP(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingVertexFormatDesc>();

    DrawingVertexFormatDesc::VertexInputElement inputElem;

    inputElem.mFormat = eFormat_R32G32B32_FLOAT;
    inputElem.mpName = strPtr("POSITION");
    inputElem.mIndex = 0;
    inputElem.mSlot = 0;
    inputElem.mOffset = 0;
    inputElem.mInstanceStepRate = 0;
    pDesc->m_inputElements.emplace_back(inputElem);

    resTable.AddResourceEntry(VertexFormatP(), pDesc);
}

void BaseRenderer::DefineVertexFormatPN(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingVertexFormatDesc>();

    DrawingVertexFormatDesc::VertexInputElement inputElem;

    inputElem.mFormat = eFormat_R32G32B32_FLOAT;
    inputElem.mpName = strPtr("POSITION");
    inputElem.mIndex = 0;
    inputElem.mSlot = 0;
    inputElem.mOffset = 0;
    inputElem.mInstanceStepRate = 0;
    pDesc->m_inputElements.emplace_back(inputElem);

    inputElem.mFormat = eFormat_R32G32B32_FLOAT;
    inputElem.mpName = strPtr("NORMAL");
    inputElem.mIndex = 0;
    inputElem.mSlot = 1;
    inputElem.mOffset = 0;
    inputElem.mInstanceStepRate = 0;
    pDesc->m_inputElements.emplace_back(inputElem);

    resTable.AddResourceEntry(VertexFormatPN(), pDesc);
}

void BaseRenderer::DefineVertexFormatPNT(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingVertexFormatDesc>();

    DrawingVertexFormatDesc::VertexInputElement inputElem;

    inputElem.mFormat = eFormat_R32G32B32_FLOAT;
    inputElem.mpName = strPtr("POSITION");
    inputElem.mIndex = 0;
    inputElem.mSlot = 0;
    inputElem.mOffset = 0;
    inputElem.mInstanceStepRate = 0;
    pDesc->m_inputElements.emplace_back(inputElem);

    inputElem.mFormat = eFormat_R32G32B32_FLOAT;
    inputElem.mpName = strPtr("NORMAL");
    inputElem.mIndex = 0;
    inputElem.mSlot = 1;
    inputElem.mOffset = 0;
    inputElem.mInstanceStepRate = 0;
    pDesc->m_inputElements.emplace_back(inputElem);

    inputElem.mFormat = eFormat_R32G32_FLOAT;
    inputElem.mpName = strPtr("TEXCOORD");
    inputElem.mIndex = 0;
    inputElem.mSlot = 2;
    inputElem.mOffset = 0;
    inputElem.mInstanceStepRate = 0;
    pDesc->m_inputElements.emplace_back(inputElem);

    resTable.AddResourceEntry(VertexFormatPNT(), pDesc);
}

void BaseRenderer::DefineStaticVertexBuffer(std::shared_ptr<std::string> pName, uint32_t stride, uint32_t count, const void* data, uint32_t size, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingVertexBufferDesc>();

    pDesc->mSizeInBytes = stride * count;
    pDesc->mStrideInBytes = stride;
    pDesc->mUsage = eUsage_Default;
    pDesc->mAccess = eAccess_No_Access;
    pDesc->mFlags = 0;

    if (resTable.AddResourceEntry(pName, pDesc))
    {
        auto pEntry = resTable.GetResourceEntry(pName);
        if (pEntry != nullptr)
        {
            pEntry->SetInitData(0, data, size);
            pEntry->SetInitDataSlices(1);
        }
    }
}

void BaseRenderer::DefineStaticIndexBuffer(std::shared_ptr<std::string> pName, uint32_t count, const void* data, uint32_t size, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingIndexBufferDesc>();

    pDesc->mSizeInBytes = 2 * count;
    pDesc->mStrideInBytes = 2;
    pDesc->mUsage = eUsage_Default;
    pDesc->mAccess = eAccess_No_Access;
    pDesc->mFlags = 0;

    if (resTable.AddResourceEntry(pName, pDesc))
    {
        auto pEntry = resTable.GetResourceEntry(pName);
        if (pEntry != nullptr)
        {
            pEntry->SetInitData(0, data, size);
            pEntry->SetInitDataSlices(1);
        }
    }
}

void BaseRenderer::DefineDynamicVertexBuffer(std::shared_ptr<std::string> pName, uint32_t stride, uint32_t count, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingVertexBufferDesc>();

    pDesc->mSizeInBytes = stride * count;
    pDesc->mStrideInBytes = stride;
    pDesc->mUsage = eUsage_Dynamic;
    pDesc->mAccess = eAccess_Write; 
    pDesc->mFlags = 0;

    resTable.AddResourceEntry(pName, pDesc);
}

void BaseRenderer::DefineDynamicIndexBuffer(std::shared_ptr<std::string> pName, uint32_t count, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingIndexBufferDesc>();

    pDesc->mSizeInBytes = 2 * count;
    pDesc->mStrideInBytes = 2;
    pDesc->mUsage = eUsage_Dynamic;
    pDesc->mAccess = eAccess_Write;
    pDesc->mFlags = 0;

    resTable.AddResourceEntry(pName, pDesc);
}

void BaseRenderer::DefineWorldMatrixConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gWorldMatrix");
    param.mType = EParam_Float4x4;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(DefaultWorldMatrix(), pDesc);
}

void BaseRenderer::DefineViewMatrixConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gViewMatrix");
    param.mType = EParam_Float4x4;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(DefaultViewMatrix(), pDesc);
}

void BaseRenderer::DefineProjectionMatrixConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gProjectionView");
    param.mType = EParam_Float4x4;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(DefaultProjectionMatrix(), pDesc);
}

void BaseRenderer::DefineCameraDirVectorConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gCameraDir");
    param.mType = EParam_Float3;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(CameraDirVector(), pDesc);
}

void BaseRenderer::DefineLightDirVectorConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gLightDir");
    param.mType = EParam_Float3;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(LightDirVector(), pDesc);
}

void BaseRenderer::DefineLightViewMatrixConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gLightViewMatrix");
    param.mType = EParam_Float4x4;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(LightViewMatrix(), pDesc);
}

void BaseRenderer::DefineLightProjMatrixConstantBuffer(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gLightProjMatrix");
    param.mType = EParam_Float4x4;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(LightProjMatrix(), pDesc);
}

void BaseRenderer::DefineDefaultDepthState(DrawingResourceTable& resTable)
{
    auto pDesc1 = std::make_shared<DrawingDepthStateDesc>();
    pDesc1->mDepthState.mDepthEnable = true;
    pDesc1->mDepthState.mDepthWriteEnable = true;
    pDesc1->mDepthState.mDepthFunc = eComparison_Less;

    pDesc1->mStencilState.mStencilEnable = false;
    pDesc1->mStencilState.mStencilReadMask = 0;
    pDesc1->mStencilState.mStencilWriteMask = 0;

    pDesc1->mStencilState.mFrontFace.mStencilPassOp = eStencilOp_Keep;
    pDesc1->mStencilState.mFrontFace.mStencilFailOp = eStencilOp_Keep;
    pDesc1->mStencilState.mFrontFace.mStencilDepthFailOp = eStencilOp_Keep;
    pDesc1->mStencilState.mFrontFace.mStencilFunc = eComparison_Always;

    pDesc1->mStencilState.mBackFace.mStencilPassOp = eStencilOp_Keep;
    pDesc1->mStencilState.mBackFace.mStencilFailOp = eStencilOp_Keep;
    pDesc1->mStencilState.mBackFace.mStencilDepthFailOp = eStencilOp_Keep;
    pDesc1->mStencilState.mBackFace.mStencilFunc = eComparison_Always;

    resTable.AddResourceEntry(DefaultDepthState(), pDesc1);

    auto pDesc2 = std::make_shared<DrawingDepthStateDesc>();
    pDesc2->mDepthState.mDepthEnable = true;
    pDesc2->mDepthState.mDepthWriteEnable = false;
    pDesc2->mDepthState.mDepthFunc = eComparison_LessEqual;

    pDesc2->mStencilState.mStencilEnable = false;
    pDesc2->mStencilState.mStencilReadMask = 0;
    pDesc2->mStencilState.mStencilWriteMask = 0;

    pDesc2->mStencilState.mFrontFace.mStencilPassOp = eStencilOp_Keep;
    pDesc2->mStencilState.mFrontFace.mStencilFailOp = eStencilOp_Keep;
    pDesc2->mStencilState.mFrontFace.mStencilDepthFailOp = eStencilOp_Keep;
    pDesc2->mStencilState.mFrontFace.mStencilFunc = eComparison_Always;

    pDesc2->mStencilState.mBackFace.mStencilPassOp = eStencilOp_Keep;
    pDesc2->mStencilState.mBackFace.mStencilFailOp = eStencilOp_Keep;
    pDesc2->mStencilState.mBackFace.mStencilDepthFailOp = eStencilOp_Keep;
    pDesc2->mStencilState.mBackFace.mStencilFunc = eComparison_Always;

    resTable.AddResourceEntry(DepthStateNoWrite(), pDesc2);

    auto pDesc3 = std::make_shared<DrawingDepthStateDesc>();
    pDesc3->mDepthState.mDepthEnable = false;
    pDesc3->mDepthState.mDepthWriteEnable = false;
    pDesc3->mDepthState.mDepthFunc = eComparison_Never;

    pDesc3->mStencilState.mStencilEnable = false;
    pDesc3->mStencilState.mStencilReadMask = 0;
    pDesc3->mStencilState.mStencilWriteMask = 0;

    pDesc3->mStencilState.mFrontFace.mStencilPassOp = eStencilOp_Keep;
    pDesc3->mStencilState.mFrontFace.mStencilFailOp = eStencilOp_Keep;
    pDesc3->mStencilState.mFrontFace.mStencilDepthFailOp = eStencilOp_Keep;
    pDesc3->mStencilState.mFrontFace.mStencilFunc = eComparison_Always;

    pDesc3->mStencilState.mBackFace.mStencilPassOp = eStencilOp_Keep;
    pDesc3->mStencilState.mBackFace.mStencilFailOp = eStencilOp_Keep;
    pDesc3->mStencilState.mBackFace.mStencilDepthFailOp = eStencilOp_Keep;
    pDesc3->mStencilState.mBackFace.mStencilFunc = eComparison_Always;

    resTable.AddResourceEntry(DepthStateDisable(), pDesc3);
}

void BaseRenderer::DefineDefaultBlendState(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingBlendStateDesc>();

    pDesc->mAlphaToCoverageEnable = false;
    pDesc->mIndependentBlendEnable = false;

    for (uint32_t i = 0; i < MAX_TARGETS; ++i)
    {
        pDesc->mTargets[i].mBlendEnable = true;

        pDesc->mTargets[i].mColorBlend.mBlendSrc = eBlend_SrcAlpha;
        pDesc->mTargets[i].mColorBlend.mBlendDst = eBlend_InvSrcAlpha;
        pDesc->mTargets[i].mColorBlend.mBlendOp = eBlendOp_Add;

        pDesc->mTargets[i].mAlphaBlend.mBlendSrc = eBlend_SrcAlpha;
        pDesc->mTargets[i].mAlphaBlend.mBlendDst = eBlend_InvSrcAlpha;
        pDesc->mTargets[i].mAlphaBlend.mBlendOp = eBlendOp_Add;

        pDesc->mTargets[i].mRenderTargetWriteMask = DrawingBlendStateDesc::BlendTarget::WriteMast_All;
    }

    resTable.AddResourceEntry(DefaultBlendState(), pDesc);
}

void BaseRenderer::DefineDefaultRasterState(DrawingResourceTable& resTable)
{
    auto pDesc1 = std::make_shared<DrawingRasterStateDesc>();
    pDesc1->mAntialiasedLineEnable = gpGlobal->GetConfiguration<GraphicsConfiguration>().GetMSAA() != eMSAA_Disable;
    pDesc1->mMultisampleEnable = gpGlobal->GetConfiguration<GraphicsConfiguration>().GetMSAA() != eMSAA_Disable;
    pDesc1->mDepthClipEnable = true;

    pDesc1->mDepthBiasClamp = 0.0f;
    pDesc1->mSlopeScaledDepthBias = 0.0f;
    pDesc1->mDepthBias = 0;

    pDesc1->mCullMode = eCullMode_Back;
    pDesc1->mFillMode = eFillMode_Solid;

    pDesc1->mFrontCounterClockwise = false;
    pDesc1->mScissorEnable = false;

    resTable.AddResourceEntry(DefaultRasterState(), pDesc1);

    auto pDesc2 = std::make_shared<DrawingRasterStateDesc>();
    pDesc2->mAntialiasedLineEnable = gpGlobal->GetConfiguration<GraphicsConfiguration>().GetMSAA() != eMSAA_Disable;
    pDesc2->mMultisampleEnable = gpGlobal->GetConfiguration<GraphicsConfiguration>().GetMSAA() != eMSAA_Disable;
    pDesc2->mDepthClipEnable = true;

    pDesc2->mDepthBiasClamp = 0.0f;
    pDesc2->mSlopeScaledDepthBias = 0.0f;
    pDesc2->mDepthBias = 0;

    pDesc2->mCullMode = eCullMode_Front;
    pDesc2->mFillMode = eFillMode_Solid;

    pDesc2->mFrontCounterClockwise = false;
    pDesc2->mScissorEnable = false;

    resTable.AddResourceEntry(RasterStateFrontCull(), pDesc2);
}

void BaseRenderer::DefineTarget(std::shared_ptr<std::string> pName, uint32_t width, uint32_t height, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingTargetDesc>();

    pDesc->mWidth = width;
    pDesc->mHeight = height;
    pDesc->mFormat = eFormat_R8G8B8A8_UNORM;

    resTable.AddResourceEntry(pName, pDesc);
}

void BaseRenderer::DefineTarget(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingTargetDesc>();

    pDesc->mWidth = gpGlobal->GetConfiguration<AppConfiguration>().GetWidth();
    pDesc->mHeight = gpGlobal->GetConfiguration<AppConfiguration>().GetHeight();
    pDesc->mFormat = eFormat_R8G8B8A8_UNORM;

    resTable.AddResourceEntry(pName, pDesc);
}

void BaseRenderer::DefineDepthBuffer(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingDepthBufferDesc>();

    pDesc->mWidth = gpGlobal->GetConfiguration<AppConfiguration>().GetWidth();
    pDesc->mHeight = gpGlobal->GetConfiguration<AppConfiguration>().GetHeight();
    pDesc->mFormat = eFormat_R24G8_TYPELESS;

    resTable.AddResourceEntry(pName, pDesc);
}

void BaseRenderer::DefineExternalTarget(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingTargetDesc>();

    pDesc->SetIsExternalResource(true);

    resTable.AddResourceEntry(pName, pDesc);
}

void BaseRenderer::DefineExternalDepthBuffer(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingDepthBufferDesc>();

    pDesc->SetIsExternalResource(true);

    resTable.AddResourceEntry(pName, pDesc);
}

void BaseRenderer::DefineExternalTexture(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingTextureDesc>();

    pDesc->SetIsExternalResource(true);

    resTable.AddResourceEntry(pName, pDesc);
}

bool BaseRenderer::DefineDynamicTexture(std::shared_ptr<std::string> pName, EDrawingFormatType format, uint32_t elementCount, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingTextureDesc>();

    uint32_t texelBytes = m_pDevice->FormatBytes(format);
    uint32_t rowCount = elementCount / DYNAMIC_TEX_ROW_SIZE;
    if ((elementCount % DYNAMIC_TEX_ROW_SIZE) != 0)
        rowCount += 1;

    pDesc->mType = eTexture_2D;
    pDesc->mFormat = format;

    pDesc->mWidth = DYNAMIC_TEX_ROW_SIZE;
    pDesc->mHeight = rowCount;
    pDesc->mDepth = 1;
    pDesc->mArraySize = 1;
    pDesc->mMipLevels = 1;

    pDesc->mBytesPerRow = texelBytes * DYNAMIC_TEX_ROW_SIZE;
    pDesc->mBytesPerSlice = texelBytes * DYNAMIC_TEX_ROW_SIZE * rowCount;

    pDesc->mUsage = eUsage_Dynamic;
    pDesc->mAccess = eAccess_Write;
    pDesc->mFlags = 0;

    if (!resTable.AddResourceEntry(pName, pDesc))
        return false;

    return true;
}

void BaseRenderer::DefineDynamicTextureWithInit(std::shared_ptr<std::string> pName, EDrawingFormatType format, uint32_t elementCount, void* pData, uint32_t size, DrawingResourceTable& resTable)
{
    if (DefineDynamicTexture(pName, format, elementCount, resTable))
    {
        auto pEntry = resTable.GetResourceEntry(pName);
        if (pEntry != nullptr)
        {
            pEntry->SetInitData(0, pData, size);
            pEntry->SetInitDataSlices(1);
        }
    }
}

void BaseRenderer::DefineVaringStates(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingVaringStatesDesc>();

    resTable.AddResourceEntry(DefaultVaringStates(), pDesc);
}

void BaseRenderer::DefinePrimitive(std::shared_ptr<std::string> pName, DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingPrimitiveDesc>();

    pDesc->mPrimitive = ePrimitive_TriangleStrip;

    resTable.AddResourceEntry(pName, pDesc);
}

void BaseRenderer::DefineLinearSampler(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingSamplerStateDesc>();

    pDesc->mSamplerMode = eSamplerMode_Compare;
    pDesc->mAddressU = eAddressMode_Wrap;
    pDesc->mAddressV = eAddressMode_Wrap;
    pDesc->mAddressW = eAddressMode_Wrap;
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

    resTable.AddResourceEntry(LinearSampler(), pDesc);
}

void BaseRenderer::BindResource(DrawingPass& pass, std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName)
{
    pass.BindResource(slotName, resName);
}

void BaseRenderer::BindEffect(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::EffectSlotName(), pName);
}

void BaseRenderer::BindVertexFormat(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::VertexFormatSlotName(), pName);
}

void BaseRenderer::BindVertexBuffer(DrawingPass& pass, uint32_t index, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::VertexBufferSlotName(index), pName);
}

void BaseRenderer::BindIndexBuffer(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::IndexBufferSlotName(), pName);
}

void BaseRenderer::BindTarget(DrawingPass& pass, uint32_t index, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::TargetSlotName(index), pName);
}

void BaseRenderer::BindDepthBuffer(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::DepthBufferSlotName(), pName);
}

void BaseRenderer::BindDepthState(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::DepthStateSlotName(), pName);
}

void BaseRenderer::BindBlendState(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::BlendStateSlotName(), pName);
}

void BaseRenderer::BindRasterState(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::RasterStateSlotName(), pName);
}

void BaseRenderer::BindPrimitive(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::PrimitiveSlotName(), pName);
}

void BaseRenderer::BindVaringStates(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::VaringStatesSlotName(), pName);
}

void BaseRenderer::AddConstantSlot(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.AddResourceSlot(pName, ResourceSlot_ConstBuffer);
    pass.BindResource(pName, pName);
}

void BaseRenderer::AddTextureSlot(DrawingPass& pass, std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pParamName)
{
    pass.AddResourceSlot(pName, ResourceSlot_Texture, pParamName);
}

void BaseRenderer::BindStaticInputsP(DrawingPass& pass)
{
    BindVertexFormat(pass, VertexFormatP());
    BindVertexBuffer(pass, 0, DefaultStaticPositionBuffer());
    BindIndexBuffer(pass, DefaultStaticIndexBuffer());
}

void BaseRenderer::BindDynamicInputsP(DrawingPass& pass)
{
    BindVertexFormat(pass, VertexFormatP());
    BindVertexBuffer(pass, 0, DefaultDynamicPositionBuffer());
    BindIndexBuffer(pass, DefaultDynamicIndexBuffer());
}

void BaseRenderer::BindStaticInputsPN(DrawingPass& pass)
{
    BindVertexFormat(pass, VertexFormatPN());
    BindVertexBuffer(pass, 0, DefaultStaticPositionBuffer());
    BindVertexBuffer(pass, 1, DefaultStaticNormalBuffer());
    BindIndexBuffer(pass, DefaultStaticIndexBuffer());
}

void BaseRenderer::BindDynamicInputsPN(DrawingPass& pass)
{
    BindVertexFormat(pass, VertexFormatPN());
    BindVertexBuffer(pass, 0, DefaultDynamicPositionBuffer());
    BindVertexBuffer(pass, 1, DefaultDynamicNormalBuffer());
    BindIndexBuffer(pass, DefaultDynamicIndexBuffer());
}

void BaseRenderer::BindStaticInputsPNT(DrawingPass& pass)
{
    BindVertexFormat(pass, VertexFormatPNT());
    BindVertexBuffer(pass, 0, DefaultStaticPositionBuffer());
    BindVertexBuffer(pass, 1, DefaultStaticNormalBuffer());
    BindVertexBuffer(pass, 2, DefaultStaticTexcoordBuffer());
    BindIndexBuffer(pass, DefaultStaticIndexBuffer());
}

void BaseRenderer::BindDynamicInputsPNT(DrawingPass& pass)
{
    BindVertexFormat(pass, VertexFormatPNT());
    BindVertexBuffer(pass, 0, DefaultDynamicPositionBuffer());
    BindVertexBuffer(pass, 1, DefaultDynamicNormalBuffer());
    BindVertexBuffer(pass, 2, DefaultDynamicTexcoordBuffer());
    BindIndexBuffer(pass, DefaultDynamicIndexBuffer());
}

void BaseRenderer::BindStaticInputsT(DrawingPass& pass)
{
    BindVertexFormat(pass, VertexFormatT());
    BindVertexBuffer(pass, 0, DefaultStaticTexcoordBuffer());
}

void BaseRenderer::BindDynamicInputsT(DrawingPass& pass)
{
    BindVertexFormat(pass, VertexFormatT());
    BindVertexBuffer(pass, 0, DefaultDynamicTexcoordBuffer());
}

void BaseRenderer::BindStates(DrawingPass& pass)
{
    BindDepthState(pass, DefaultDepthState());
    BindBlendState(pass, DefaultBlendState());
    BindRasterState(pass, DefaultRasterState());
}

void BaseRenderer::BindOutput(DrawingPass& pass)
{
    BindTarget(pass, 0, ScreenTarget());
    BindDepthBuffer(pass, ScreenDepthBuffer());
}

void BaseRenderer::BindConstants(DrawingPass& pass)
{
    AddConstantSlot(pass, DefaultWorldMatrix());
    AddConstantSlot(pass, DefaultViewMatrix());
    AddConstantSlot(pass, DefaultProjectionMatrix());
}

void BaseRenderer::BindCameraConstants(DrawingPass& pass)
{
    AddConstantSlot(pass, CameraDirVector());
}

void BaseRenderer::BindLightConstants(DrawingPass& pass)
{
    AddConstantSlot(pass, LightDirVector());
    AddConstantSlot(pass, LightViewMatrix());
    AddConstantSlot(pass, LightProjMatrix());
}

void BaseRenderer::BindShadowMapTexture(DrawingPass& pass)
{
    auto shadowmap_tex_slot = strPtr("ShadowMapTex");
    AddTextureSlot(pass, shadowmap_tex_slot, strPtr("gShadowMapTexture"));
    BindResource(pass, shadowmap_tex_slot, ShadowMapTexture());

    auto shadowmap_sampler_slot = strPtr("ShadowMapSampler");
    pass.AddResourceSlot(shadowmap_sampler_slot, ResourceSlot_Sampler, strPtr("gShadowMapSampler"));
    BindResource(pass, shadowmap_sampler_slot, ShadowMapSampler());
}

void BaseRenderer::BindScreenSpaceShadowTexture(DrawingPass& pass)
{
    auto screenspaceshadow_tex_slot = strPtr("ScreenSpaceShadowTex");
    AddTextureSlot(pass, screenspaceshadow_tex_slot, strPtr("gScreenSpaceShadowTexture"));
    BindResource(pass, screenspaceshadow_tex_slot, ScreenSpaceShadowTexture());
}

void BaseRenderer::BindBaseColorTexture(DrawingPass& pass)
{
    auto basecolor_tex_slot = strPtr("BaseColorTex");
    AddTextureSlot(pass, basecolor_tex_slot, strPtr("gBaseColorTexture"));
    BindResource(pass, basecolor_tex_slot, BaseColorTexture());
}

void BaseRenderer::BindOcclusionTexture(DrawingPass& pass)
{
    auto occlusion_tex_slot = strPtr("OcclusionTex");
    AddTextureSlot(pass, occlusion_tex_slot, strPtr("gOcclusionTexture"));
    BindResource(pass, occlusion_tex_slot, OcclusionTexture());
}

void BaseRenderer::BindMetallicRoughnessTexture(DrawingPass& pass)
{
    auto metallicroughness_tex_slot = strPtr("MetallicRoughnessTex");
    AddTextureSlot(pass, metallicroughness_tex_slot, strPtr("gMetallicRoughnessTexture"));
    BindResource(pass, metallicroughness_tex_slot, MetallicRoughnessTexture());
}

void BaseRenderer::BindNormalTexture(DrawingPass& pass)
{
    auto normal_tex_slot = strPtr("NormalTex");
    AddTextureSlot(pass, normal_tex_slot, strPtr("gNormalTexture"));
    BindResource(pass, normal_tex_slot, NormalTexture());
}

void BaseRenderer::BindEmissiveTexture(DrawingPass& pass)
{
    auto emissive_tex_slot = strPtr("EmissiveTex");
    AddTextureSlot(pass, emissive_tex_slot, strPtr("gEmissiveTexture"));
    BindResource(pass, emissive_tex_slot, EmissiveTexture());
}

void BaseRenderer::BindDepthTexture(DrawingPass& pass)
{
    auto depth_tex_slot = strPtr("DepthTex");
    AddTextureSlot(pass, depth_tex_slot, strPtr("gDepthTexture"));
    BindResource(pass, depth_tex_slot, ScreenDepthTexture());
}

void BaseRenderer::BindRectTexture(DrawingPass& pass)
{
    auto rect_tex_slot = strPtr("RectTex");
    AddTextureSlot(pass, rect_tex_slot, strPtr("gRectTexture"));
    BindResource(pass, rect_tex_slot, RectTexture());
}

void BaseRenderer::BindLinearSampler(DrawingPass& pass)
{
    auto linear_sampler_slot = strPtr("LinearSampler");
    pass.AddResourceSlot(linear_sampler_slot, ResourceSlot_Sampler, strPtr("gLinearSampler"));
    BindResource(pass, linear_sampler_slot, LinearSampler());
}

void BaseRenderer::DefineShaderResource(DrawingResourceTable& resTable)
{
    DefineVertexShader(BasicVertexShader(), strPtr("Asset\\Shader\\HLSL\\basic.vs"), strPtr("Basic_VS"), resTable);
    DefinePixelShader(BasicPixelShader(), strPtr("Asset\\Shader\\HLSL\\basic.ps"), strPtr("Basic_PS"), resTable);
    DefineLinkedEffect(BasicEffect(), BasicVertexShader(), BasicPixelShader(), resTable);

    DefineVertexShader(ScreenSpaceShadowVertexShader(), strPtr("Asset\\Shader\\HLSL\\screen_space_shadow.vs"), strPtr("ScreenSpaceShadow_VS"), resTable);
    DefinePixelShader(ScreenSpaceShadowPixelShader(), strPtr("Asset\\Shader\\HLSL\\screen_space_shadow.ps"), strPtr("ScreenSpaceShadow_PS"), resTable);
    DefineLinkedEffect(ScreenSpaceShadowEffect(), ScreenSpaceShadowVertexShader(), ScreenSpaceShadowPixelShader(), resTable);

    DefineVertexShader(RectVertexShader(), strPtr("Asset\\Shader\\HLSL\\rect.vs"), strPtr("Rect_VS"), resTable);
    DefinePixelShader(SSAOPixelShader(), strPtr("Asset\\Shader\\HLSL\\ssao.ps"), strPtr("SSAO_PS"), resTable);
    DefineLinkedEffect(SSAOEffect(), RectVertexShader(), SSAOPixelShader(), resTable);

    DefinePixelShader(RectPixelShader(), strPtr("Asset\\Shader\\HLSL\\rect.ps"), strPtr("Rect_PS"), resTable);
    DefineLinkedEffect(RectEffect(), RectVertexShader(), RectPixelShader(), resTable);
}

void BaseRenderer::DefineShadowCasterBlendState(DrawingResourceTable& resTable)
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

void BaseRenderer::DefineShadowMapSampler(DrawingResourceTable& resTable)
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

void BaseRenderer::CreateDepthTextureTarget()
{
    std::shared_ptr<DrawingTexture> pTexture = nullptr;

    auto pDepthBuffer = m_pDeviceContext->GetDepthBuffer();
    assert(pDepthBuffer != nullptr);

    m_pDevice->CreateTexture(DrawingTextureDesc(), pTexture, pDepthBuffer);
    assert(pTexture != nullptr);

    m_pDepthBuffer = std::make_shared<DrawingTextureDepthBuffer>(m_pDevice, pDepthBuffer, pTexture);
}

void BaseRenderer::CreateShadowmapTextureTarget()
{
    m_pShadowMap = std::make_shared<DrawingTextureTarget>(m_pDevice);
    m_pShadowMap->Initialize(gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), gpGlobal->GetConfiguration<AppConfiguration>().GetHeight(), eFormat_R32_FLOAT);
}

void BaseRenderer::CreateScreenSpaceShadowTextureTarget()
{
    m_pScreenSpaceShadow = std::make_shared<DrawingTextureTarget>(m_pDevice);
    m_pScreenSpaceShadow->Initialize(gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), gpGlobal->GetConfiguration<AppConfiguration>().GetHeight(), eFormat_R8G8B8A8_UNORM);
}

void BaseRenderer::CreateSSAOTextureTarget()
{
    m_pSSAOTexture = std::make_shared<DrawingTextureTarget>(m_pDevice);
    m_pSSAOTexture->Initialize(gpGlobal->GetConfiguration<AppConfiguration>().GetWidth(), gpGlobal->GetConfiguration<AppConfiguration>().GetHeight(), eFormat_R8G8B8A8_UNORM);
}

void BaseRenderer::UpdatePrimitive(DrawingResourceTable& resTable)
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

void BaseRenderer::UpdateRectPrimitive(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(RectPrimitive());
    if (pEntry == nullptr)
        return;

    auto pPrimitive = std::dynamic_pointer_cast<DrawingPrimitive>(pEntry->GetResource());
    if (pPrimitive == nullptr)
        return;

    pPrimitive->SetPrimitiveType(ePrimitive_TriangleStrip);
    pPrimitive->SetVertexCount(3);
    pPrimitive->SetIndexCount(0);
    pPrimitive->SetInstanceCount(0);

    pPrimitive->SetVertexOffset(0);
    pPrimitive->SetIndexOffset(0);
    pPrimitive->SetInstanceOffset(0);
}

float4x4 BaseRenderer::UpdateWorldMatrix(const TransformComponent* pTransform)
{
    float3 position = pTransform->GetPosition();
    float3 rotate = pTransform->GetRotate();
    float3 scale = pTransform->GetScale();
    float4 quaternion = pTransform->GetQuaternion();

    float4x4 posMatrix = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        position.x, position.y, position.z, 1.f
    };

    auto rotMat = Mat::EulerRotateLH(rotate.x, rotate.y, rotate.z);
    float4x4 rotMatrix = {
        rotMat.x00, rotMat.x01, rotMat.x02, 0.f,
        rotMat.x10, rotMat.x11, rotMat.x12, 0.f,
        rotMat.x20, rotMat.x21, rotMat.x22, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    auto quatMat = Mat::QuatRotateLH(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    float4x4 quatMatrix = {
        quatMat.x00, quatMat.x01, quatMat.x02, 0.f,
        quatMat.x10, quatMat.x11, quatMat.x12, 0.f,
        quatMat.x20, quatMat.x21, quatMat.x22, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    float4x4 scaleMatrix = {
        scale.x, 0.f, 0.f, 0.f,
        0.f, scale.y, 0.f, 0.f,
        0.f, 0.f, scale.z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    return Mat::Mul(scaleMatrix, Mat::Mul(quatMatrix, Mat::Mul(rotMatrix, posMatrix)));
}