#include "PolylineRenderer.h"
#include "DrawingDevice.h"

using namespace Engine;

uint32_t PolylineRenderer::m_sLineVertexID[LINE_MAX_VERTEX_COUNT] = { 0 };
uint32_t PolylineRenderer::m_sLineInstanceID[LINE_MAX_INSTANCE_COUNT] = { 0 };

PolylineRenderer::PolylineRenderer() : BaseRenderer()
{
    InitLineVertexID();
    InitLineInstanceID();
    m_instanceCount = 0;

    m_pData = new char[65535];
}

void PolylineRenderer::Initialize()
{
}

void PolylineRenderer::Shutdown()
{
}

void PolylineRenderer::Tick(float elapsedTime)
{
}

void PolylineRenderer::DefineResources(DrawingResourceTable& resTable)
{
    DefineDefaultResources(resTable);
    DefineShaderResource(resTable);
    DefinePipelineStateResource(resTable);

    DefineNullInputVertexFormat(resTable);
    DefineStaticVertexBuffer(VB_VertexID(), 4, LINE_MAX_VERTEX_COUNT, &m_sLineVertexID, LINE_MAX_VERTEX_COUNT * 4, resTable);
    DefineStaticVertexBuffer(VB_InstanceID(), 4, LINE_MAX_INSTANCE_COUNT, &m_sLineInstanceID, LINE_MAX_INSTANCE_COUNT * 4, resTable);

    DefineScreenConstants(resTable);
    DefineLineSmoothingConstants(resTable);

    DefineDynamicTextureWithInit(LineSegmentTex(), eFormat_R32G32B32_FLOAT, MAX_PRIM_ELEMENT_COUNT * SIGNLE_SEGMENT_TEX_ELEMENT_COUNT, m_pData, m_size, resTable);
}

void PolylineRenderer::SetupStages()
{
    auto pStage = CreateStage(LineStage());
    pStage->AppendDrawingPass(CreateSegmentPass(LineSegmentPass(), LineSegmentEffect()));

    m_stageTable.AddDrawingStage(pStage->GetName(), pStage);
}

void PolylineRenderer::SetupBuffers(DrawingResourceTable& resTable)
{
}

void PolylineRenderer::Cleanup()
{
}

void PolylineRenderer::BeginFrame()
{
}

void PolylineRenderer::EndFrame()
{
}

void PolylineRenderer::UpdatePrimitive(DrawingResourceTable& resTable)
{
    UpdateConstants(resTable);

    auto pEntry = resTable.GetResourceEntry(DefaultPrimitive());
    if (pEntry == nullptr)
        return;

    auto pPrimitive = std::dynamic_pointer_cast<DrawingPrimitive>(pEntry->GetResource());
    if (pPrimitive == nullptr)
        return;

    pPrimitive->SetPrimitiveType(ePrimitive_TriangleStrip);
    pPrimitive->SetVertexCount(m_instanceCount > 0 ? 4U : 0);
    pPrimitive->SetIndexCount(0);
    pPrimitive->SetInstanceCount(m_instanceCount);

    pPrimitive->SetVertexOffset(0);
    pPrimitive->SetIndexOffset(0);
    pPrimitive->SetInstanceOffset(0);
}

void PolylineRenderer::Draw(DrawingResourceTable& resTable)
{
    UpdatePrimitive(resTable);
    FlushStage(LineStage());
}

void PolylineRenderer::AttachSegment(PolylineRendererComponent::LineSegmentGeom geom)
{
    uint32_t size = sizeof(PolylineRendererComponent::LineSegmentGeom);
    memcpy((char*)m_pData + m_size, &geom, size);

    m_size += size;
    m_instanceCount++;
}

void PolylineRenderer::InitLineVertexID()
{
    for (uint32_t i = 0; i < LINE_MAX_VERTEX_COUNT; ++i)
        m_sLineVertexID[i] = i;
}

void PolylineRenderer::InitLineInstanceID()
{
    for (uint32_t i = 0; i < LINE_MAX_INSTANCE_COUNT; ++i)
        m_sLineInstanceID[i] = i;
}

void PolylineRenderer::DefineShaderResource(DrawingResourceTable& resTable)
{
    DefineVertexShader(LineSegmentVertexShader(), strPtr("Asset\\Shader\\HLSL\\Geometry\\segment.vs"), strPtr("Segment_VS"), resTable);
    DefinePixelShader(LineSegmentPixelShader(), strPtr("Asset\\Shader\\HLSL\\Geometry\\segment.ps"), strPtr("Segment_PS"), resTable);

    DefineLinkedEffect(LineSegmentEffect(), LineSegmentVertexShader(), LineSegmentPixelShader(), resTable);
}

void PolylineRenderer::DefinePipelineStateResource(DrawingResourceTable& resTable)
{
    DefinePipelineState(NullInputVertexFormat(),
                        LineSegmentPipelineState(),
                        DefaultPrimitive(),
                        LineSegmentEffect(),
                        DefaultBlendState(),
                        DefaultRasterState(),
                        DefaultDepthState(),
                        ScreenTarget(),
                        resTable);
}

void PolylineRenderer::DefineNullInputVertexFormat(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingVertexFormatDesc>();

    DrawingVertexFormatDesc::VertexInputElement inputElem;

    inputElem.mFormat = eFormat_R32_SINT;
    inputElem.mpName = strPtr("TEXCOORD");
    inputElem.mIndex = 0;
    inputElem.mSlot = 0;
    inputElem.mOffset = 0;
    inputElem.mInstanceStepRate = 0;
    pDesc->m_inputElements.emplace_back(inputElem);

    inputElem.mFormat = eFormat_R32_SINT;
    inputElem.mpName = strPtr("TEXCOORD");
    inputElem.mIndex = 1;
    inputElem.mSlot = 1;
    inputElem.mOffset = 0;
    inputElem.mInstanceStepRate = 1;
    pDesc->m_inputElements.emplace_back(inputElem);

    resTable.AddResourceEntry(NullInputVertexFormat(), pDesc);
}

void PolylineRenderer::DefineScreenConstants(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gInvScreenSize");
    param.mType = EParam_Float2;
    pDesc->mParameters.emplace_back(param);

    param.mpName = strPtr("gScreenSize");
    param.mType = EParam_Float2;
    pDesc->mParameters.emplace_back(param);

    param.mpName = strPtr("gLCSMatrix");
    param.mType = EParam_Float4x4;
    pDesc->mParameters.emplace_back(param);

    param.mpName = strPtr("gLCSCenterX");
    param.mType = EParam_Int;
    pDesc->mParameters.emplace_back(param);

    param.mpName = strPtr("gLCSCenterY");
    param.mType = EParam_Int;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(ScreenConstants(), pDesc);
}

void PolylineRenderer::DefineLineSmoothingConstants(DrawingResourceTable& resTable)
{
    auto pDesc = std::make_shared<DrawingConstantBufferDesc>();

    DrawingConstantBufferDesc::ParamDesc param;
    param.mpName = strPtr("gGaussTheta");
    param.mType = EParam_Float;
    pDesc->mParameters.emplace_back(param);

    param.mpName = strPtr("gGaussSigma");
    param.mType = EParam_Float;
    pDesc->mParameters.emplace_back(param);

    resTable.AddResourceEntry(LineSmoothingConstants(), pDesc);
}

void PolylineRenderer::BindLineSegmentInputs(DrawingPass& pass)
{
    BindVertexFormat(pass, NullInputVertexFormat());
    BindVertexBuffer(pass, 0, VB_VertexID());
    BindVertexBuffer(pass, 1, VB_InstanceID());
}

void PolylineRenderer::BindLineSegmentTexture(DrawingPass& pass, std::shared_ptr<std::string> pName)
{
    auto vertex_tex_slot = strPtr("VertexTex");
    AddTextureSlot(pass, vertex_tex_slot, strPtr("gLineSegmentTex"));
    BindResource(pass, vertex_tex_slot, pName);
}

void PolylineRenderer::BindLineSegmentConstants(DrawingPass& pass)
{
    AddConstantSlot(pass, ScreenConstants());
    AddConstantSlot(pass, LineSmoothingConstants());
}

std::shared_ptr<DrawingPass> PolylineRenderer::CreateSegmentPass(
    std::shared_ptr<std::string> pPassName,
    std::shared_ptr<std::string> pEffectName)
{
    auto pPass = CreatePass(pPassName);

    BindEffect(*pPass, pEffectName);
    BindPipelineState(*pPass, LineSegmentPipelineState());
    BindLineSegmentInputs(*pPass);
    BindStates(*pPass);
    BindOutput(*pPass);
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());
    BindLineSegmentConstants(*pPass);
    BindLineSegmentTexture(*pPass, LineSegmentTex());

    return pPass;
}

void PolylineRenderer::UpdateConstants(DrawingResourceTable& resTable)
{
    auto pEntry = resTable.GetResourceEntry(ScreenConstants());
    assert(pEntry != nullptr);
    auto pCB = std::dynamic_pointer_cast<DrawingConstantBuffer>(pEntry->GetResource());
    if (pCB == nullptr)
        return;
    auto pParam = pCB->GetParameter(strPtr("gInvScreenSize"));
    if (pParam != nullptr)
    {
        float width = (float)gpGlobal->GetConfiguration().width;
        float height = (float)gpGlobal->GetConfiguration().height;
        float2 invScreenSize = float2(1.0f / width, 1.0f / height);
        pParam->AsFloat2(invScreenSize);
    }
}