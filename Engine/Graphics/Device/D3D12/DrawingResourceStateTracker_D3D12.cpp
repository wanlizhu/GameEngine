#include <d3dx12.h>
#include <algorithm>

#include "SafeQueue.h"
#include "DrawingResourceStateTracker_D3D12.h"
#include "DrawingCommandManager_D3D12.h"

using namespace Engine;

DrawingResourceStateTracker_D3D12::ResourceStates DrawingResourceStateTracker_D3D12::s_gStates;
std::mutex DrawingResourceStateTracker_D3D12::m_mutex;

void DrawingResourceStateTracker_D3D12::ResourceBarrier(std::shared_ptr<DrawingCommandList_D3D12> pCommandList, const D3D12_RESOURCE_BARRIER& barrier)
{
    auto& barriers = m_barriersTable[pCommandList];

    if (barrier.Type != D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
        barriers.emplace_back(barrier);
    else
    {
        const auto& transition = barrier.Transition;
        auto& states = m_statesTable[pCommandList];
        auto it = states.find(transition.pResource);
        if (it == states.cend())
        {
            auto& pendingBarriers = m_pendingBarriersTable[pCommandList];
            pendingBarriers.emplace_back(barrier);
        }
        else
        {
            auto& resourceState = it->second;
            if (transition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES && !resourceState.mStateTable.empty())
            {
                for (auto subresourceState : resourceState.mStateTable)
                {
                    if (transition.StateAfter != subresourceState.second )
                    {
                        D3D12_RESOURCE_BARRIER newBarrier = barrier;
                        newBarrier.Transition.Subresource = subresourceState.first;
                        newBarrier.Transition.StateBefore = subresourceState.second;
                        barriers.emplace_back(newBarrier);
                    }
                }
            }
            else
            {
                auto finalState = resourceState.GetSubresourceState(transition.Subresource);
                if (transition.StateAfter != finalState)
                {
                    D3D12_RESOURCE_BARRIER newBarrier = barrier;
                    newBarrier.Transition.StateBefore = finalState;
                    barriers.emplace_back(newBarrier);
                }
            }
        }
        states[transition.pResource].SetSubresourceState(transition.Subresource, transition.StateAfter);
    }
}

void DrawingResourceStateTracker_D3D12::TransitionBarrier(std::shared_ptr<DrawingCommandList_D3D12> pCommandList, std::shared_ptr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateAfter, UINT subResource)
{
    ResourceBarrier(pCommandList, CD3DX12_RESOURCE_BARRIER::Transition(pResource.get(), D3D12_RESOURCE_STATE_COMMON, stateAfter, subResource));
}

void DrawingResourceStateTracker_D3D12::AliasBarrier(std::shared_ptr<DrawingCommandList_D3D12> pCommandList, std::shared_ptr<ID3D12Resource> pResourceBefore, std::shared_ptr<ID3D12Resource> pResourceAfter)
{
    ResourceBarrier(pCommandList, CD3DX12_RESOURCE_BARRIER::Aliasing(pResourceBefore ? pResourceBefore.get() : nullptr, pResourceAfter ? pResourceAfter.get() : nullptr));
}

void DrawingResourceStateTracker_D3D12::UAVBarrier(std::shared_ptr<DrawingCommandList_D3D12> pCommandList, std::shared_ptr<ID3D12Resource> pResource)
{
    ResourceBarrier(pCommandList, CD3DX12_RESOURCE_BARRIER::UAV(pResource ? pResource.get() : nullptr));
}

void DrawingResourceStateTracker_D3D12::FlushBarriers(std::shared_ptr<DrawingCommandList_D3D12> pCommandList)
{
    auto& barriers = m_barriersTable[pCommandList];
    UINT numBarriers = static_cast<UINT>(barriers.size());
    if (numBarriers > 0 )
    {
        pCommandList->GetCommandList()->ResourceBarrier(numBarriers, barriers.data());
        barriers.clear();
    }
}

bool DrawingResourceStateTracker_D3D12::FlushPendingBarriers(std::shared_ptr<DrawingCommandList_D3D12> pCommandList, std::shared_ptr<DrawingCommandList_D3D12> pPendingCommandList)
{
    auto& pendingBarriers = m_pendingBarriersTable[pCommandList];
    ResourceBarriers barriers;

    for (auto& pendingBarrier : pendingBarriers)
    {
        if (pendingBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
        {
            auto& pendingTransition = pendingBarrier.Transition;
            const auto& it = s_gStates.find(pendingTransition.pResource);
            if (it != s_gStates.cend())
            {
                auto& resourceState = it->second;
                if (pendingTransition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES && !resourceState.mStateTable.empty())
                {
                    for (const auto& subresourceState : resourceState.mStateTable)
                    {
                        if (pendingTransition.StateAfter != subresourceState.second)
                        {
                            D3D12_RESOURCE_BARRIER newBarrier = pendingBarrier;
                            newBarrier.Transition.Subresource = subresourceState.first;
                            newBarrier.Transition.StateBefore = subresourceState.second;
                            barriers.emplace_back(newBarrier);
                        }
                    }
                }
                else
                {
                    const auto& finalState = resourceState.GetSubresourceState(pendingTransition.Subresource);
                    if (pendingTransition.StateAfter != finalState)
                    {
                        pendingBarrier.Transition.StateBefore = finalState;
                        barriers.emplace_back(pendingBarrier);
                    }
                }
            }
        }
    }

    UINT numBarriers = static_cast<UINT>(barriers.size());
    if (numBarriers > 0)
        pPendingCommandList->GetCommandList()->ResourceBarrier(numBarriers, barriers.data());

    pendingBarriers.clear();
    return numBarriers > 0;
}

void DrawingResourceStateTracker_D3D12::CommitFinalResourceStates(std::shared_ptr<DrawingCommandList_D3D12> pCommandList)
{
    auto& resourceStates = m_statesTable[pCommandList];

    std::for_each(resourceStates.begin(), resourceStates.end(), [this](auto& resourceState)
    {
        s_gStates[resourceState.first] = resourceState.second;
    });

    resourceStates.clear();
}

void DrawingResourceStateTracker_D3D12::AddGlobalResourceState(std::shared_ptr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES state)
{
    if (pResource != nullptr)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        s_gStates[pResource.get()].SetSubresourceState(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
    }
}

void DrawingResourceStateTracker_D3D12::RemoveGlobalResourceState(std::shared_ptr<ID3D12Resource> pResource)
{
    if (pResource != nullptr)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        s_gStates.erase(pResource.get());
    }
}