#pragma once

#include <d3d12.h>
#include <thread>
#include <mutex>

#include "DrawingResourceStateTracker_D3D12.h"
#include "DrawingConstants.h"

namespace Engine
{
    class DrawingDevice_D3D12;
    class DrawingCommandManager_D3D12
    {
    public:
        DrawingCommandManager_D3D12(const std::shared_ptr<DrawingDevice_D3D12> device, EDrawingCommandListType type);
        virtual ~DrawingCommandManager_D3D12();

        std::shared_ptr<ID3D12GraphicsCommandList> GetCommandList(bool bForceNew = false);
        std::shared_ptr<ID3D12CommandQueue> GetCommandQueue() const;

        uint64_t ExecuteCommandLists();

        uint64_t Signal();
        bool IsFenceComplete(uint64_t fenceValue);
        void WaitForFenceValue(uint64_t fenceValue);
        void Flush();

        void TransitionBarrier(std::shared_ptr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateAfter, bool bForceFlush = false);
        void FlushBarriers();

    protected:
        std::shared_ptr<ID3D12CommandAllocator> CreateCommandAllocator();
        std::shared_ptr<ID3D12GraphicsCommandList> CreateCommandList(std::shared_ptr<ID3D12CommandAllocator> allocator);

        void CommandListInFlightProcess();

    private:
        struct CommandAllocatorEntry
        {
            uint64_t fenceValue;
            std::shared_ptr<ID3D12CommandAllocator> m_pCommandAllocator;
        };

        struct CommandListEntry
        {
            std::shared_ptr<ID3D12GraphicsCommandList> m_pCommandList;
            std::shared_ptr<ID3D12CommandAllocator> m_pCommandAllocator;
        };

        typedef SafeQueue<CommandAllocatorEntry> CommandAllocatorEntryQueueType;
        typedef SafeQueue<std::shared_ptr<ID3D12CommandAllocator>> CommandAllocatorQueueType;
        typedef SafeQueue<std::shared_ptr<ID3D12GraphicsCommandList>> CommandListQueueType;
        typedef std::unordered_multimap<std::thread::id, CommandListEntry> CommandListTableType;

        EDrawingCommandListType m_type;

        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        std::shared_ptr<ID3D12CommandQueue> m_pCommandQueue;
        std::shared_ptr<ID3D12Fence> m_pFence;
        HANDLE m_pFenceEvent;
        uint64_t m_fenceValue;

        std::shared_ptr<DrawingResourceStateTracker_D3D12> m_pResourceStateTracker;

        CommandAllocatorEntryQueueType m_commandAllocatorInFlightQueue;

        CommandAllocatorQueueType m_commandAllocatorAwaitQueue;
        CommandListQueueType m_commandListAwaitQueue;

        CommandListTableType m_commandListTable;

        bool m_isCommandListInFlightThreadRun;
        std::thread m_commandListInFlightThread;
        mutable std::mutex m_mutex;
    };
}
