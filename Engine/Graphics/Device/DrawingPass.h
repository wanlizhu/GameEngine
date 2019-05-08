#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "DrawingResourceTable.h"
#include "DrawingConstants.h"

namespace Engine
{
    class DrawingDevice;
    class DrawingContext;
    class DrawingEffect;
    class DrawingTarget;
    class DrawingDepthBuffer;
    class DrawingRWBuffer;
    class DrawingVertexFormat;
    class DrawingIndexBuffer;
    class DrawingVertexBuffer;
    class DrawingPrimitive;
    enum EResourceSlotType
    {
        ResourceSlot_Unknown = 0,
        ResourceSlot_Static,
        ResourceSlot_Texture,
        ResourceSlot_TexBuffer,
        ResourceSlot_RWBuffer_Input,
        ResourceSlot_RWBuffer_Output,
        ResourceSlot_ConstBuffer,
        ResourceSlot_Sampler,
    };
    class DrawingPass
    {
    public:
        DrawingPass(std::shared_ptr<std::string> pName, const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingPass();

        static std::shared_ptr<std::string> EffectSlotName();
        static std::shared_ptr<std::string> VertexFormatSlotName();
        static std::shared_ptr<std::string> VertexBufferSlotName(uint32_t index);
        static std::shared_ptr<std::string> IndexBufferSlotName();
        static std::shared_ptr<std::string> BlendStateSlotName();
        static std::shared_ptr<std::string> RasterStateSlotName();
        static std::shared_ptr<std::string> DepthStateSlotName();
        static std::shared_ptr<std::string> TargetSlotName(uint32_t index);
        static std::shared_ptr<std::string> DepthBufferSlotName();
        static std::shared_ptr<std::string> RWBufferSlotName(uint32_t index);
        static std::shared_ptr<std::string> PrimitiveSlotName();
        static std::shared_ptr<std::string> VaringStatesSlotName();
        static std::shared_ptr<std::string> PipelineStateSlotName();

        std::shared_ptr<std::string> GetName() const;

        bool AddResourceSlot(std::shared_ptr<std::string> slotName, EResourceSlotType type, std::shared_ptr<std::string> key = nullptr);
        bool RemoveResourceSlot(std::shared_ptr<std::string> slotName);

        bool BindResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName);
        bool UnBindResource(std::shared_ptr<std::string> slotName);

        void FetchResources(DrawingResourceTable& resTable);
        void ClearResources();

        void UpdateStaticResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName, DrawingResourceTable& resTable);
        void UpdateDynamicResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName, DrawingResourceTable& resTable);

        const std::shared_ptr<DrawingResourceTable::ResourceEntry> GetStaticResourceEntry(std::shared_ptr<std::string> slotName) const;
        const std::shared_ptr<DrawingResourceTable::ResourceEntry> GetDynamicResourceEntry(std::shared_ptr<std::string> slotName) const;

        bool Flush(DrawingContext& dc);

    private:
        bool LoadEffect();

        void UpdateInputs();
        void UpdateOutputs();
        void UpdateViewport();
        void UpdateStates();
        void UpdateScissorBox();

        void UpdateConstants();
        void UpdateTextures();
        void UpdateTexBuffers();
        void UpdateRWBuffers();
        void UpdateBuffers();
        void UpdateSamplers();

        void PushStates();
        void PopStates();

        void UpdatePipelineState();
        void BeginEffect(DrawingContext& dc);
        void EndEffect(DrawingContext& dc);
        bool DrawPrimitive(DrawingContext& dc);

    protected:
        class ResourceSlot
        {
        public:
            ResourceSlot();
            ResourceSlot(uint32_t val);
            ResourceSlot(std::shared_ptr<std::string> slotName, EResourceSlotType type, std::shared_ptr<std::string> key);
            ~ResourceSlot();

            std::shared_ptr<std::string> mpName;
            std::shared_ptr<std::string> mpResName;
            std::shared_ptr<DrawingResourceTable::ResourceEntry> mpRes;
            EResourceSlotType mType;
            std::shared_ptr<std::string> mpKey;
        };

        class ResourceSlotTable
        {
        public:
            ResourceSlotTable();
            ~ResourceSlotTable();

            bool AddResourceSlot(std::shared_ptr<std::string> slotName, EResourceSlotType type, std::shared_ptr<std::string> key = nullptr);
            bool RemoveResourceSlot(std::shared_ptr<std::string> slotName);

            void FetchResources(DrawingResourceTable& resTable);
            void UpdateSingleResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName, DrawingResourceTable& resTable);
            void ClearResources();
            void Clear();

            bool BindResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName);
            bool UnBindResource(std::shared_ptr<std::string> slotName);

            const std::shared_ptr<DrawingResourceTable::ResourceEntry> GetResourceEntry(std::shared_ptr<std::string> slotName) const;
            static std::shared_ptr<DrawingResource> GetSlotDeviceResource(const ResourceSlot* pSlot);

            typedef std::unordered_map<std::shared_ptr<std::string>, ResourceSlot> ResourceSlotTableType;

        protected:
            ResourceSlotTableType mSlotTable;
        };

        class DynamicResourceSlotTable : public ResourceSlotTable
        {
        public:
            DynamicResourceSlotTable();
            ~DynamicResourceSlotTable();

            void UpdateConstants(std::shared_ptr<DrawingEffect> pEffect);
            void UpdateTextures(std::shared_ptr<DrawingEffect> pEffect);
            void UpdateTexBuffers(std::shared_ptr<DrawingEffect> pEffect);
            void UpdateRWBuffers(std::shared_ptr<DrawingEffect> pEffect);
            void UpdateBuffers(std::shared_ptr<DrawingEffect> pEffect);
            void UpdateSamplers(std::shared_ptr<DrawingEffect> pEffect);
        };

        class StaticResourceSlotTable : public ResourceSlotTable
        {
        public:
            StaticResourceSlotTable();
            ~StaticResourceSlotTable();

            static std::shared_ptr<std::string> GetStaticSlotName(const char* name, uint32_t id, int32_t sub = -1);

            static uint32_t GetEffectSlotID();
            static uint32_t GetVertexFormatSlotID();
            static uint32_t GetVertexBufferSlotID(uint32_t index);
            static uint32_t GetIndexBufferSlotID();
            static uint32_t GetBlendStateSlotID();
            static uint32_t GetRasterStateSlotID();
            static uint32_t GetDepthStateSlotID();
            static uint32_t GetTargetSlotID(uint32_t index);
            static uint32_t GetDepthBufferSlotID();
            static uint32_t GetRWBufferSlotID(uint32_t index);
            static uint32_t GetPrimitiveSlotID();
            static uint32_t GetVaringStatesSlotID();
            static uint32_t GetPipelineStateSlotID();

            void UpdateVertexFormat(const std::shared_ptr<DrawingDevice>& device);
            void UpdateVertexBuffer(const std::shared_ptr<DrawingDevice>& device);
            void UpdateIndexBuffer(const std::shared_ptr<DrawingDevice>& device);
            void UpdateTargets(const std::shared_ptr<DrawingDevice>& device);
            void UpdateBlendState(const std::shared_ptr<DrawingDevice>& device);
            void UpdateDepthState(const std::shared_ptr<DrawingDevice>& device);
            void UpdateRasterState(const std::shared_ptr<DrawingDevice>& device);
            void UpdateViewport(const std::shared_ptr<DrawingDevice>& device);
            void UpdateScissorBox(const std::shared_ptr<DrawingDevice>& device);
            void UpdatePipelineState(const std::shared_ptr<DrawingDevice>& device);
            void RestoreViewport(const std::shared_ptr<DrawingDevice>& device);
            void RestoreScissorBox(const std::shared_ptr<DrawingDevice>& device);

            std::shared_ptr<DrawingEffect> LoadEffect();
            std::shared_ptr<DrawingPrimitive> LoadPrimitive();

        private:
            enum StaticSlotIndex
            {
                Effect_ID = 0,

                Vertex_Format_ID,
                Vertex_Buffer_Start,
                Vertex_Buffer_End = Vertex_Buffer_Start + MAX_VERTEX_STREAM,

                Index_Buffer_ID,

                Blend_State_ID,
                Raster_State_ID,
                Depth_State_ID,

                Target_Start,
                Target_End = Target_Start + MAX_TARGETS,

                DepthBuffer_ID,

                RWBuffer_Start,
                RWBuffer_End = RWBuffer_Start + MAX_RW_BUFFER,

                Prim_Info_ID,
                Varing_State_ID,
                Indirect_Buffer_ID,

                Pipeline_State_ID,

                Max_Static_Slot,
            };

            static std::shared_ptr<std::string> sResourceSlotName[Max_Static_Slot];

            void AddStaticResourceSlot();
            void AddStaticResourceSlot(std::shared_ptr<std::string> slotName);

            void LoadTargets(std::shared_ptr<DrawingTarget> targets[], uint32_t& targetCount);
            std::shared_ptr<DrawingDepthBuffer> LoadDepthBuffer();
            void LoadRWBuffers(std::shared_ptr<DrawingRWBuffer> rwbuffers[], uint32_t& bufferCount);

            std::shared_ptr<DrawingVertexFormat> LoadVertexFormat();
            void LoadVertexBuffer(std::shared_ptr<DrawingVertexBuffer> vbs[], uint32_t& vbCount);
            std::shared_ptr<DrawingIndexBuffer> LoadIndexBuffer();
        };

        std::shared_ptr<std::string> m_pName;
        std::shared_ptr<std::string> m_pEffectName;

        std::shared_ptr<DrawingEffect> m_pEffect;
        std::shared_ptr<DrawingDevice> m_pDevice;

        DynamicResourceSlotTable m_dynamicTable;
        StaticResourceSlotTable m_staticTable;
    };
}