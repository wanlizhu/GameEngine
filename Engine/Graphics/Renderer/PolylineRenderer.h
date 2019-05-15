#pragma

#include "BaseRenderer.h"
#include "PolylineRendererComponent.h"

namespace Engine
{
    class PolylineRenderer final : public BaseRenderer
    {
    public:
        PolylineRenderer();
        virtual ~PolylineRenderer() {}

        void Initialize() override;
        void Shutdown() override;

        void Tick(float elapsedTime) override;

        void DefineResources(DrawingResourceTable& resTable) override;
        void SetupStages() override;
        void SetupBuffers(DrawingResourceTable& resTable) override;
        void Cleanup() override;

        void BeginFrame() override;
        void EndFrame() override;

        void UpdatePrimitive(DrawingResourceTable& resTable) override;
        void Draw(DrawingResourceTable& resTable) override;

        void AttachSegment(PolylineRendererComponent::LineSegmentGeom geom);

    public:
        // Define vertex format names
        FuncResourceName(NullInputVertexFormat);
        // Define vertex buffer names
        FuncResourceName(VB_VertexID);
        FuncResourceName(VB_InstanceID);
        // Define constant buffer names
        FuncResourceName(ScreenConstants);
        FuncResourceName(LineSmoothingConstants);
        // Define shader resource names
        FuncResourceName(LineSegmentVertexShader);
        FuncResourceName(LineSegmentPixelShader);
        // Define pipeline state names
        FuncResourceName(LineSegmentPipelineState);
        // Define effect resource names
        FuncResourceName(LineSegmentEffect);
        // Define stage resource names
        FuncResourceName(LineStage);
        // Define effect resource names
        FuncResourceName(LineSegmentPass);
        // Define geometry texture names
        FuncResourceName(LineSegmentTex);

    protected:
        static const uint32_t MAX_PRIM_ELEMENT_COUNT = 16384;
        static const uint32_t SIGNLE_SEGMENT_TEX_ELEMENT_COUNT = 2;

        static const uint32_t LINE_MAX_VERTEX_COUNT = 65536;
        static const uint32_t LINE_MAX_INSTANCE_COUNT = 65536;

        static uint32_t m_sLineVertexID[LINE_MAX_VERTEX_COUNT];
        static uint32_t m_sLineInstanceID[LINE_MAX_INSTANCE_COUNT];

        static void InitLineVertexID();
        static void InitLineInstanceID();

        uint32_t m_instanceCount;

        void* m_pData = nullptr;
        uint32_t m_size = 0;

        void DefineShaderResource(DrawingResourceTable& resTable);
        void DefinePipelineStateResource(DrawingResourceTable& resTable);
        void DefineNullInputVertexFormat(DrawingResourceTable& resTable);
        void DefineScreenConstants(DrawingResourceTable& resTable);
        void DefineLineSmoothingConstants(DrawingResourceTable& resTable);

        void BindLineSegmentInputs(DrawingPass& pass);
        void BindLineSegmentTexture(DrawingPass& pass, std::shared_ptr<std::string> pName);
        void BindLineSegmentConstants(DrawingPass& pass);

        void UpdateConstants(DrawingResourceTable& resTable);

    private:
        std::shared_ptr<DrawingPass> CreateSegmentPass(
            std::shared_ptr<std::string> pPassName,
            std::shared_ptr<std::string> pEffectName);
    };
}