#include "ForwardRenderer.h"
#include "DrawingDevice.h"

using namespace Engine;

ForwardRenderer::ForwardRenderer() : BaseRenderer()
{
}

void ForwardRenderer::Initialize()
{
}

void ForwardRenderer::Shutdown()
{
}

void ForwardRenderer::Tick(float elapsedTime)
{
}

void ForwardRenderer::DefineResources(DrawingResourceTable& resTable)
{
    DefineDefaultResources(resTable);
    DefineShaderResource(resTable);
    DefinePipelineStateResource(resTable);
}

void ForwardRenderer::SetupStages()
{
    auto pStage = CreateStage(BasicPrimitiveStage());
    pStage->AppendDrawingPass(CreateDefaultPass(BasicPrimitiveDefaultPass(), BasicPrimitiveEffect(), BasicPrimitiveIndexBuffer()));

    m_stageTable.AddDrawingStage(pStage->GetName(), pStage);
}

void ForwardRenderer::SetupBuffers(DrawingResourceTable& resTable)
{
}

void ForwardRenderer::Cleanup()
{
}

void ForwardRenderer::BeginFrame()
{
}

void ForwardRenderer::EndFrame()
{
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
    pPrimitive->SetVertexCount(m_vertexCount);
    pPrimitive->SetIndexCount(m_indexCount);
    pPrimitive->SetInstanceCount(0);

    pPrimitive->SetVertexOffset(0);
    pPrimitive->SetIndexOffset(0);
    pPrimitive->SetInstanceOffset(0);
}

void ForwardRenderer::Draw(DrawingResourceTable& resTable)
{
    UpdatePrimitive(resTable);
    FlushStage(BasicPrimitiveStage());
}

void ForwardRenderer::DefineShaderResource(DrawingResourceTable& resTable)
{
    DefineVertexShader(BasicPrimitiveVertexShader(), strPtr("Asset\\Shader\\HLSL\\basic.vs"), strPtr("BasicPrimitive_VS"), resTable);
    DefinePixelShader(BasicPrimitivePixelShader(), strPtr("Asset\\Shader\\HLSL\\basic.ps"), strPtr("BasicPrimitive_PS"), resTable);

    DefineLinkedEffect(BasicPrimitiveEffect(), BasicPrimitiveVertexShader(), BasicPrimitivePixelShader(), resTable);
}

void ForwardRenderer::DefinePipelineStateResource(DrawingResourceTable& resTable)
{
    DefinePipelineState(DefaultVertexFormat(),
                        BasicPrimitivePipelineState(),
                        DefaultPrimitive(),
                        BasicPrimitiveEffect(),
                        DefaultBlendState(),
                        DefaultRasterState(),
                        DefaultDepthState(),
                        ScreenTarget(),
                        resTable);
}

std::shared_ptr<DrawingPass> ForwardRenderer::CreateDefaultPass(
    std::shared_ptr<std::string> pPassName,
    std::shared_ptr<std::string> pEffectName,
    std::shared_ptr<std::string> pIndexName)
{
    auto pPass = CreatePass(pPassName);

    BindEffect(*pPass, pEffectName);
    BindPipelineState(*pPass, BasicPrimitivePipelineState());
    BindInputs(*pPass);
    BindStates(*pPass);
    BindOutput(*pPass);
    BindPrimitive(*pPass, DefaultPrimitive());
    BindVaringStates(*pPass, DefaultVaringStates());

    BindConstants(*pPass);

    return pPass;
}