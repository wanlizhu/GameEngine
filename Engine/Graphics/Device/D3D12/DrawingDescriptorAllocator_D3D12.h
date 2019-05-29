#pragma once

#include <d3d12.h>
#include <memory>
#include <mutex>
#include <vector>
#include <set>
#include <map>

namespace Engine
{
    class DrawingDevice_D3D12;
    class DrawingDescriptorAllocator_D3D12
    {
    private:
        struct Page;
    public:
        DrawingDescriptorAllocator_D3D12(const std::shared_ptr<DrawingDevice_D3D12> pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerPage = 256);
        virtual ~DrawingDescriptorAllocator_D3D12();

        struct Allocation
        {
            Allocation() : m_numHandles(0), m_size(0), m_pPage(nullptr), m_pCPUHandle{0} {}
            Allocation(D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32_t numHandles, uint32_t size, std::shared_ptr<Page> pPage) :
                m_pCPUHandle(handle), m_numHandles(numHandles), m_size(size), m_pPage(pPage) {}

            ~Allocation() {}

            bool IsValid()
            {
                return m_pCPUHandle.ptr != 0;
            }

            uint32_t m_numHandles;
            uint32_t m_size;

            std::shared_ptr<Page> m_pPage;
            D3D12_CPU_DESCRIPTOR_HANDLE m_pCPUHandle;
        };

        Allocation Allocate(uint32_t numDescriptors = 1);

    private:
        struct Page : public std::enable_shared_from_this<Page>
        {
            Page(const std::weak_ptr<DrawingDevice_D3D12> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
            ~Page();

            bool HasEnoughSpace(uint32_t numDescriptors) const;
            D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeap() const;
            uint32_t GetNumFreeHandles() const;
            Allocation Allocate(uint32_t numDescriptors);

        protected:
            void AddBlock(uint32_t offset, uint32_t numDescriptors);

        private:
            struct BlockInfo;
            typedef std::map<uint32_t, BlockInfo> BlockListByOffset;
            typedef std::multimap<uint32_t, BlockListByOffset::iterator> BlockListBySize;

            struct BlockInfo
            {
                BlockInfo(uint32_t size) : m_size(size) {}
                uint32_t m_size;
                BlockListBySize::iterator m_blockListBySizeIt;
            };

            std::weak_ptr<DrawingDevice_D3D12> m_pDevice;

            BlockListByOffset m_blockListByOffset;
            BlockListBySize m_blockListBySize;

            std::shared_ptr<ID3D12DescriptorHeap> m_pDescriptorHeap;
            D3D12_DESCRIPTOR_HEAP_TYPE m_type;
            D3D12_CPU_DESCRIPTOR_HANDLE m_currentHandle;

            uint32_t m_descriptorHandleIncrementSize;
            uint32_t m_numDescriptorsInPage;
            uint32_t m_numFreeHandles;

            mutable std::mutex m_mutex;
        };

    protected:
        typedef std::vector<std::shared_ptr<Page>> DescriptorPagePool;
        std::shared_ptr<Page> GetNewPage();

        std::weak_ptr<DrawingDevice_D3D12> m_pDevice;
        D3D12_DESCRIPTOR_HEAP_TYPE m_type;
        uint32_t m_numDescriptorsPerPage;
        DescriptorPagePool m_pagePool;
        std::set<size_t> m_availPages;

        mutable std::mutex m_mutex;
    };
}