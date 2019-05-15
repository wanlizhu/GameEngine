#pragma once

#include <memory>
#include <stdint.h>
#include <unordered_map>

namespace Engine
{
    class DrawingDevice;
    class DrawingResource;
    class DrawingResourceDesc;
    class DrawingEffectPool;
    class DrawingResourceTable;
    class DrawingResourceFactory
    {
    public:
        DrawingResourceFactory(const std::shared_ptr<DrawingDevice> pDevice);
        virtual ~DrawingResourceFactory();

        void SetEffectPool(const std::weak_ptr<DrawingEffectPool> pEffectPool);

        bool CreateResource(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, DrawingResourceTable& resTable, const void* pData[] = nullptr, uint32_t size[] = nullptr, uint32_t slices = 0) const;

        bool CreateEffect(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;
        bool CreateVertexFormat(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;
        bool CreateVertexBuffer(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, const void* pData = nullptr, uint32_t size = 0) const;
        bool CreateIndexBuffer(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, const void* pData = nullptr, uint32_t size = 0) const;
        bool CreateTexture(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, const void* pData[] = nullptr, uint32_t size[] = nullptr, uint32_t slices = 0) const;

        bool CreateConstantBuffer(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;

        bool CreateBlendState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;
        bool CreateDepthState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;
        bool CreateRasterState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;
        bool CreateSamplerState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;

        bool CreateVertexShader(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;
        bool CreatePixelShader(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;

        bool CreatePrimitive(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;
        bool CreateVaringStates(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const;

        bool CreatePipelineState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, DrawingResourceTable& resTable) const;

    private:
        std::shared_ptr<DrawingDevice> m_pDevice;
        std::weak_ptr<DrawingEffectPool> m_pEffectPool;
    };

    class DrawingResourceTable
    {
    public:
        DrawingResourceTable(const DrawingResourceFactory& factory);
        virtual ~DrawingResourceTable();

        class ResourceEntry
        {
            friend class DrawingResourceTable;
        public:
            virtual ~ResourceEntry();
            bool CreateResource();

            std::shared_ptr<DrawingResource> GetResource();
            const std::shared_ptr<DrawingResourceDesc> GetDesc() const;

            void SetDesc(std::shared_ptr<DrawingResourceDesc> pDesc);
            void SetInitData(uint32_t index, const void* pData, uint32_t size);
            void SetInitDataSlices(uint32_t slices);

            bool SetExternalResource(std::shared_ptr<DrawingResource> pRes); 

        private:
            ResourceEntry(std::shared_ptr<DrawingResourceDesc> pDesc, const DrawingResourceFactory& factory, DrawingResourceTable& table);
            void LoadPrecedingResources();

        private:
            std::shared_ptr<DrawingResourceDesc> m_pDesc;
            std::shared_ptr<DrawingResource> m_pRes;

            static const uint32_t MAX_INIT_SLICES = 256;

            const void* m_pData[MAX_INIT_SLICES] = { nullptr };
            uint32_t m_size[MAX_INIT_SLICES] = { 0 };
            uint32_t m_slices = 0;

            const DrawingResourceFactory& m_factory;
            DrawingResourceTable& m_resTable;
        };

        std::shared_ptr<ResourceEntry> GetResourceEntry(std::shared_ptr<std::string> pName) const;
        bool AddResourceEntry(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingResourceDesc> pDesc);
        bool RemoveResourceEntry(std::shared_ptr<std::string> pName);
        void ClearResourceEntries();
        bool BuildResources();

    private:
        typedef std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<ResourceEntry>> ResourceTableType;
        ResourceTableType m_resourceTable;
        const DrawingResourceFactory& m_factory;
    };
}