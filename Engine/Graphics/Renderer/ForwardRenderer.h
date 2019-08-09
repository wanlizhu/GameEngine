#pragma once

#include "BaseRenderer.h"

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

    private:
        void BeginDrawPass() override;
        void EndDrawPass() override;

        void FlushData() override;
        void ResetData() override;
        void UpdatePrimitive(DrawingResourceTable& resTable) override;

    public:
        // Define shader resource names
        FuncResourceName(BasicPrimitiveVertexShader);
        FuncResourceName(BasicPrimitivePixelShader);
        // Define pipeline state names
        FuncResourceName(BasicPrimitivePipelineState);
        // Define effect resource names
        FuncResourceName(BasicPrimitiveEffect);
        // Define effect resource names
        FuncResourceName(BasicPrimitiveDefaultPass);

    protected:
        void DefineShaderResource(DrawingResourceTable& resTable);
        void DefinePipelineStateResource(DrawingResourceTable& resTable);

    private:
        std::shared_ptr<DrawingPass> CreateForwardBasePass(
            std::shared_ptr<std::string> pPassName,
            std::shared_ptr<std::string> pEffectName);
    };
}