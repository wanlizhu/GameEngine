#pragma once

#include <d3d12.h>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace Engine
{
    class DrawingResourceStateTracker_D3D12
    {
    public:
        DrawingResourceStateTracker_D3D12() = default;
        virtual ~DrawingResourceStateTracker_D3D12() = default;

        void ResourceBarrier(std::shared_ptr<ID3D12GraphicsCommandList> pCommandList, const D3D12_RESOURCE_BARRIER& barrier);

        void TransitionBarrier(std::shared_ptr<ID3D12GraphicsCommandList> pCommandList, std::shared_ptr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateAfter, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
        void AliasBarrier(std::shared_ptr<ID3D12GraphicsCommandList> pCommandList, std::shared_ptr<ID3D12Resource> pResourceBefore = nullptr, std::shared_ptr<ID3D12Resource> pResourceAfter = nullptr);
        void UAVBarrier(std::shared_ptr<ID3D12GraphicsCommandList> pCommandList, std::shared_ptr<ID3D12Resource> pResource = nullptr);

        void FlushBarriers(std::shared_ptr<ID3D12GraphicsCommandList> pCommandList);
        bool FlushPendingBarriers(std::shared_ptr<ID3D12GraphicsCommandList> pCommandList, std::shared_ptr<ID3D12GraphicsCommandList> pPendingCommandList);
        void CommitFinalResourceStates(std::shared_ptr<ID3D12GraphicsCommandList> pCommandList);

        static void AddGlobalResourceState(std::shared_ptr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES state);
        static void RemoveGlobalResourceState(std::shared_ptr<ID3D12Resource> pResource);

    private:
        struct ResourceState
        {
            explicit ResourceState(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON) : mState(state) {}

            D3D12_RESOURCE_STATES GetSubresourceState(UINT id) const
            {
                auto it = mStateTable.find(id);
                if (it != mStateTable.cend())
                    return it->second;

                return mState;
            }

            void SetSubresourceState(UINT id, D3D12_RESOURCE_STATES state)
            {
                if (id == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
                {
                    mState = state;
                    mStateTable.clear();
                }
                else
                    mStateTable[id] = state;

            }

            D3D12_RESOURCE_STATES mState;
            std::unordered_map<UINT, D3D12_RESOURCE_STATES> mStateTable;
        };

        typedef std::vector<D3D12_RESOURCE_BARRIER> ResourceBarriers;
        typedef std::unordered_map<ID3D12Resource*, ResourceState> ResourceStates;

        typedef std::unordered_map<std::shared_ptr<ID3D12GraphicsCommandList>, ResourceBarriers> ResourceBarriersTable;
        typedef std::unordered_map<std::shared_ptr<ID3D12GraphicsCommandList>, ResourceStates> ResourceStatesTable;

        ResourceBarriersTable m_pendingBarriersTable;
        ResourceBarriersTable m_barriersTable;
        ResourceStatesTable m_statesTable;

        static ResourceStates s_gStates;
        static std::mutex m_mutex;
    };
}
