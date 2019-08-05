#pragma once

#include <d3d12.h>
#include <thread>
#include <mutex>
#include <vector>

#include "DrawingConstants.h"
#include "DrawingResourceStateTracker_D3D12.h"
#include "DrawingUploadAllocator_D3D12.h"
#include "DrawingDescriptorAllocator_D3D12.h"
#include "DrawingDynamicDescriptorHeap_D3D12.h"

namespace Engine
{
    class DrawingDevice_D3D12;
    class DrawingCommandList_D3D12 : public std::enable_shared_from_this<DrawingCommandList_D3D12>
    {
    public:
        DrawingCommandList_D3D12(const std::shared_ptr<DrawingDevice_D3D12> device, EDrawingCommandListType type);
        virtual ~DrawingCommandList_D3D12();

        void TransitionBarrier(std::shared_ptr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateAfter, bool bForceFlush = false);
        void FlushBarriers();
        void Reset();
        bool Close(std::shared_ptr<DrawingCommandList_D3D12>& pPendingCommandList);
        void Close();

        EDrawingCommandListType GetCommandListType() const;
        std::shared_ptr<ID3D12CommandAllocator> GetCommandAllocator() const;
        std::shared_ptr<ID3D12GraphicsCommandList> GetCommandList() const;

        std::shared_ptr<DrawingUploadAllocator_D3D12> GetUploadAllocator() const;
        std::shared_ptr<DrawingDescriptorAllocator_D3D12> GetDescriptorAllocator(EDrawingDescriptorHeapType type) const;
        std::shared_ptr<DrawingDynamicDescriptorHeap_D3D12> GetDynamicDescriptorHeap(EDrawingDescriptorHeapType type) const;

        DrawingUploadAllocator_D3D12::Allocation AllocationUpload(uint64_t sizeInBytes, uint64_t alignment);
        DrawingDescriptorAllocator_D3D12::Allocation AllocationDescriptors(EDrawingDescriptorHeapType type, uint32_t numDescriptors = 1);

    private:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        std::shared_ptr<ID3D12CommandAllocator> m_pCommandAllocator;
        std::shared_ptr<ID3D12GraphicsCommandList> m_pCommandList;

        EDrawingCommandListType m_type;
        std::shared_ptr<DrawingResourceStateTracker_D3D12> m_pResourceStateTracker;

        std::shared_ptr<DrawingUploadAllocator_D3D12> m_pUploadAllocator;

        std::shared_ptr<DrawingDescriptorAllocator_D3D12> m_pDescriptorAllocators[eDescriptorHeap_Count];
        std::shared_ptr<DrawingDynamicDescriptorHeap_D3D12> m_pDynamicDescriptorHeaps[eDescriptorHeap_Count];
    };

    class DrawingCommandManager_D3D12
    {
    public:
        DrawingCommandManager_D3D12(const std::shared_ptr<DrawingDevice_D3D12> device, EDrawingCommandListType type);
        virtual ~DrawingCommandManager_D3D12();

        std::shared_ptr<DrawingCommandList_D3D12> GetCommandList(bool bForceNew = false);
        std::shared_ptr<ID3D12CommandQueue> GetCommandQueue() const;

        uint64_t ExecuteAllCommandLists();
        uint64_t ExecuteCommandLists(const std::vector<std::shared_ptr<DrawingCommandList_D3D12>>& pCommandLists);

        uint64_t Signal();
        bool IsFenceComplete(uint64_t fenceValue);
        void WaitForFenceValue(uint64_t fenceValue);
        void Flush();

    protected:
        void CommandListInFlightProcess();

    private:
        struct CommandListEntry
        {
            uint64_t m_fenceValue;
            std::shared_ptr<DrawingCommandList_D3D12> m_pCommandList;
        };

        typedef SafeQueue<CommandListEntry> CommandListEntryQueueType;
        typedef SafeQueue<std::shared_ptr<DrawingCommandList_D3D12>> CommandListQueueType;
        typedef std::unordered_multimap<std::thread::id, std::shared_ptr<DrawingCommandList_D3D12>> CommandListTableType;

        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        std::shared_ptr<ID3D12CommandQueue> m_pCommandQueue;
        std::shared_ptr<ID3D12Fence> m_pFence;

        EDrawingCommandListType m_type;
        uint64_t m_fenceValue;

        CommandListEntryQueueType m_commandListInFlightQueue;
        CommandListQueueType m_commandListAwaitQueue;
        CommandListTableType m_commandListTable;

        bool m_isCommandListInFlightThreadRun;
        std::thread m_commandListInFlightThread;
        mutable std::mutex m_mutex;
    };
}
