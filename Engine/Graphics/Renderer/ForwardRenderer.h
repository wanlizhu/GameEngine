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

        void DefineShaderResource(DrawingResourceTable& resTable);

    public:
        // Define shader resource names
        FuncResourceName(ForwardShadingVertexShader)
        FuncResourceName(ForwardShadingPixelShader)
        // Define effect resource names
        FuncResourceName(ForwardShadingEffect)
        // Define pass names
        FuncResourceName(DepthPass)
        FuncResourceName(ForwardShadingPass)

    private:
        std::shared_ptr<DrawingPass> CreateDepthPass();
        std::shared_ptr<DrawingPass> CreateForwardShadingPass();
    };
}