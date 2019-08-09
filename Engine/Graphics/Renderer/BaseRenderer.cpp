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
    m_pTransientIndexBuffer = CreateTransientIndexBuffer(resTable, DefaultDynamicIndexBuffer());
}

std::shared_ptr<DrawingPass> BaseRenderer::GetPass(std::shared_ptr<std::string> pName)
{
    auto iter = m_passTable.find(pName);
    if (iter == m_passTable.end())
        return nullptr;

    return iter->second;
}

void BaseRenderer::Begin()
{
    m_renderQueue.Reset();
}

void BaseRenderer::AddRenderables(RenderQueueItemListType renderables)
{
    for (auto& item : renderables)
        item.pRenderable->GetRenderable(m_renderQueue, item.pTransformComp);
}

void BaseRenderer::Flush(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass)
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
    assert(m_pTransientIndexBuffer->CheckCapacity(indexCount));

    auto pAttributes = pMesh->GetAttributes();

    std::for_each(pAttributes.cbegin(), pAttributes.cend(), [&](std::shared_ptr<Attribute> pElem)
    {
        auto type = (uint32_t)pElem->semanticType;
        
        if (type == (uint32_t)Attribute::ESemanticType::Position)
            m_pTransientPositionBuffer->FillData(pElem->pData.get(), vertexCount);

        if (type == (uint32_t)Attribute::ESemanticType::Normal)
            m_pTransientNormalBuffer->FillData(pElem->pData.get(), vertexCount);
    });

    m_pTransientIndexBuffer->FillData(pMesh->GetIndexData().get(), indexCount);
}

void BaseRenderer::DefineDefaultResources(DrawingResourceTable& resTable)
{
    DefineDefaultVertexFormat(resTable);

    DefineDynamicVertexBuffer(DefaultDynamicPositionBuffer(), PositionOffset, MAX_VERTEX_COUNT, resTable);
    DefineDynamicVertexBuffer(DefaultDynamicNormalBuffer(), NormalOffset, MAX_VERTEX_COUNT, resTable);
    DefineDynamicIndexBuffer(DefaultDynamicIndexBuffer(), MAX_INDEX_COUNT, resTable);

    DefineWorldMatrixConstantBuffer(resTable);
    DefineViewMatrixConstantBuffer(resTable);
    DefineProjectionMatrixConstantBuffer(resTable);

    DefineExternalTarget(ScreenTarget(), resTable);
    DefineExternalDepthBuffer(ScreenDepthBuffer(), resTable);

    DefineDefaultDepthState(resTable);
    
    DefineDefaultBlendState(resTable);
    DefineDefaultRasterState(resTable);

    DefinePrimitive(DefaultPrimitive(), resTable);
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

void BaseRenderer::DefinePipelineState(std::shared_ptr<std::string> pVertexFormatName,
                                       std::shared_ptr<std::string> pPipelineStateName, 
                                       std::shared_ptr<std::string> pPrimitiveName,
                                       std::shared_ptr<std::string> pEffectName,
                                       std::shared_ptr<std::string> pBlendStateName,
                                       std::shared_ptr<std::string> pRasterStateName,
                                       std::shared_ptr<std::string> pDepthStencilStateName,
                                       std::shared_ptr<std::string> pRenderTargetName,
                                       DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingPipelineStateDesc>();

    pDesc->AttachSubobject(DrawingPipelineStateDesc::ePipelineStateSubobjectType_InputLayout, pVertexFormatName);
    pDesc->AttachSubobject(DrawingPipelineStateDesc::ePipelineStateSubobjectType_PrimitiveTopology, pPrimitiveName);
    pDesc->AttachSubobject(DrawingPipelineStateDesc::ePipelineStateSubobjectType_Effect, pEffectName);
    pDesc->AttachSubobject(DrawingPipelineStateDesc::ePipelineStateSubobjectType_BlendState, pBlendStateName);
    pDesc->AttachSubobject(DrawingPipelineStateDesc::ePipelineStateSubobjectType_RasterState, pRasterStateName);
    pDesc->AttachSubobject(DrawingPipelineStateDesc::ePipelineStateSubobjectType_DepthStencilState, pDepthStencilStateName);
    pDesc->AttachSubobject(DrawingPipelineStateDesc::ePipelineStateSubobjectType_RenderTarget, pRenderTargetName);

    resTable.AddResourceEntry(pPipelineStateName, pDesc);
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

void BaseRenderer::DefineDefaultVertexFormat(DrawingResourceTable& resTable)
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

    resTable.AddResourceEntry(DefaultVertexFormat(), pDesc);
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

void BaseRenderer::DefineDefaultDepthState(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingDepthStateDesc>();

    pDesc->mDepthState.mDepthEnable = true;
    pDesc->mDepthState.mDepthWriteEnable = true;
    pDesc->mDepthState.mDepthFunc = eComparison_Less;

    pDesc->mStencilState.mStencilEnable = false;
    pDesc->mStencilState.mStencilReadMask = 0;
    pDesc->mStencilState.mStencilWriteMask = 0;

    pDesc->mStencilState.mFrontFace.mStencilPassOp = eStencilOp_Keep;
    pDesc->mStencilState.mFrontFace.mStencilFailOp = eStencilOp_Keep;
    pDesc->mStencilState.mFrontFace.mStencilDepthFailOp = eStencilOp_Keep;
    pDesc->mStencilState.mFrontFace.mStencilFunc = eComparison_Always;

    pDesc->mStencilState.mBackFace.mStencilPassOp = eStencilOp_Keep;
    pDesc->mStencilState.mBackFace.mStencilFailOp = eStencilOp_Keep;
    pDesc->mStencilState.mBackFace.mStencilDepthFailOp = eStencilOp_Keep;
    pDesc->mStencilState.mBackFace.mStencilFunc = eComparison_Always;

    resTable.AddResourceEntry(DefaultDepthState(), pDesc);
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
    auto pDesc = std::make_shared<DrawingRasterStateDesc>();

    pDesc->mAntialiasedLineEnable = gpGlobal->GetConfiguration<GraphicsConfiguration>().GetMSAA() != eMSAA_Disable;
    pDesc->mMultisampleEnable = gpGlobal->GetConfiguration<GraphicsConfiguration>().GetMSAA() != eMSAA_Disable;
    pDesc->mDepthClipEnable = true;

    pDesc->mDepthBiasClamp = 0.0f;
    pDesc->mSlopeScaledDepthBias = 0.0f;
    pDesc->mDepthBias = 0;

    pDesc->mCullMode = eCullMode_Back;
    pDesc->mFillMode = eFillMode_Solid;

    pDesc->mFrontCounterClockwise = false;
    pDesc->mScissorEnable = false;

    resTable.AddResourceEntry(DefaultRasterState(), pDesc);
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

void BaseRenderer::BindResource(DrawingPass& pass, std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName)
{
    pass.BindResource(slotName, resName);
}

void BaseRenderer::BindEffect(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::EffectSlotName(), pName);
}

void BindPipeline(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::PipelineStateSlotName(), pName);
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

void BaseRenderer::BindPipelineState(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    pass.BindResource(DrawingPass::PipelineStateSlotName(), pName);
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

void BaseRenderer::BindStaticInputs(DrawingPass& pass)
{
    BindVertexFormat(pass, DefaultVertexFormat());
    BindVertexBuffer(pass, 0, DefaultStaticPositionBuffer());
    BindVertexBuffer(pass, 1, DefaultStaticNormalBuffer());
    BindIndexBuffer(pass, DefaultStaticIndexBuffer());
}

void BaseRenderer::BindDynamicInputs(DrawingPass& pass)
{
    BindVertexFormat(pass, DefaultVertexFormat());
    BindVertexBuffer(pass, 0, DefaultDynamicPositionBuffer());
    BindVertexBuffer(pass, 1, DefaultDynamicNormalBuffer());
    BindIndexBuffer(pass, DefaultDynamicIndexBuffer());
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

std::shared_ptr<DrawingPass> BaseRenderer::CreatePass(std::shared_ptr<std::string> pName)
{
    return std::make_shared<DrawingPass>(pName, m_pDevice);
}

float4x4 BaseRenderer::UpdateWorldMatrix(const TransformComponent* pTransform)
{
    float3 position = pTransform->GetPosition();
    float3 rotate = pTransform->GetRotate();
    float3 scale = pTransform->GetScale();

    float cosR = std::cosf(rotate.y);
    float sinR = std::sinf(rotate.y);

    float4x4 posMatrix = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        position.x, position.y, position.z, 1.f
    };

    float4x4 rotMatrix = {
        cosR, 0.f, sinR, 0.f,
        0.f, 1.f, 0.f, 0.f,
        -sinR, 0.f, cosR, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    float4x4 scaleMatrix = {
        scale.x, 0.f, 0.f, 0.f,
        0.f, scale.y, 0.f, 0.f,
        0.f, 0.f, scale.z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    return Mat::Mul(scaleMatrix, Mat::Mul(rotMatrix, posMatrix));
}