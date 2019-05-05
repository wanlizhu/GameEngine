#pragma once

#include "BaseRenderer.h"

namespace Engine
{
    class BasicPrimitiveRenderer final : public BaseRenderer
    {
    public:
        BasicPrimitiveRenderer();
        virtual ~BasicPrimitiveRenderer() {}

        void Initialize() override;
        void Shutdown() override;

        void Tick() override;

        void DefineResources(DrawingResourceTable& resTable) override;
        void SetupStages() override;
        void SetupBuffers(DrawingResourceTable& resTable) override;
        void Cleanup() override;

        void BeginFrame() override;
        void EndFrame() override;

        void UpdatePrimitive(DrawingResourceTable& resTable) override;
        void Draw(DrawingResourceTable& resTable) override;

    public:
        // Define shader resource names
        FuncResourceName(BasicPrimitiveVertexShader);
        FuncResourceName(BasicPrimitivePixelShader);
        // Define pipeline state names
        FuncResourceName(BasicPrimitivePipelineState);
        // Define effect resource names
        FuncResourceName(BasicPrimitiveEffect);
        // Define stage resource names
        FuncResourceName(BasicPrimitiveStage);
        // Define effect resource names
        FuncResourceName(BasicPrimitiveDefaultPass);
        // Define index buffer resource names
        FuncResourceName(BasicPrimitiveIndexBuffer);

    protected:
        void DefineShaderResource(DrawingResourceTable& resTable);
        void DefinePipelineStateResource(DrawingResourceTable& resTable);

    private:
        std::shared_ptr<DrawingPass> CreateDefaultPass(
            std::shared_ptr<std::string> pPassName,
            std::shared_ptr<std::string> pEffectName,
            std::shared_ptr<std::string> pIndexName);
    };
}