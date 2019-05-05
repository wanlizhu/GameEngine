#include "DrawingDevice_D3D12.h"
#include "DrawingCommandManager_D3D12.h"

using namespace Engine;

DrawingCommandManager_D3D12::DrawingCommandManager_D3D12(const std::shared_ptr<DrawingDevice_D3D12> device, EDrawingCommandListType type) :
    m_pDevice(device), m_type(type), m_fenceValue(0), m_isCommandListInFlightThreadRun(true)
{
    D3D12_COMMAND_QUEUE_DESC desc;
    desc.Type = D3D12Enum(m_type);
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ID3D12CommandQueue* pCommandQueueRaw;
    HRESULT hr = m_pDevice->GetDevice()->CreateCommandQueue(&desc, __uuidof(ID3D12CommandQueue), (void**)&pCommandQueueRaw);
    assert(SUCCEEDED(hr));
    m_pCommandQueue = std::shared_ptr<ID3D12CommandQueue>(pCommandQueueRaw, D3D12Releaser<ID3D12CommandQueue>);

    ID3D12Fence* pFenceRaw;
    hr = m_pDevice->GetDevice()->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&pFenceRaw);
    assert(SUCCEEDED(hr));
    m_pFence = std::shared_ptr<ID3D12Fence>(pFenceRaw, D3D12Releaser<ID3D12Fence>);
    m_pFenceEvent = ::CreateEvent(nullptr, false, false, nullptr);

    m_pResourceStateTracker = std::make_shared<DrawingResourceStateTracker_D3D12>();

    m_commandListInFlightThread = std::thread(&DrawingCommandManager_D3D12::CommandListInFlightProcess, this);
}

DrawingCommandManager_D3D12::~DrawingCommandManager_D3D12()
{
    ::CloseHandle(m_pFenceEvent);
    m_isCommandListInFlightThreadRun = false;
}

std::shared_ptr<ID3D12GraphicsCommandList> DrawingCommandManager_D3D12::GetCommandList(bool bForceNew)
{
    std::thread::id threadID = std::this_thread::get_id();
    auto it = m_commandListTable.find(threadID);
    if (it != m_commandListTable.cend() && !bForceNew)
        return it->second.m_pCommandList;

    std::shared_ptr<ID3D12CommandAllocator> pCommandAllocator;
    std::shared_ptr<ID3D12GraphicsCommandList> pCommandList;

    if (!m_commandAllocatorAwaitQueue.Empty())
        m_commandAllocatorAwaitQueue.TryPop(pCommandAllocator);
    else
        pCommandAllocator = CreateCommandAllocator();

    if (!m_commandListAwaitQueue.Empty())
    {
        m_commandListAwaitQueue.TryPop(pCommandList);
        assert(SUCCEEDED(pCommandList->Reset(pCommandAllocator.get(), nullptr)));
    }
    else
        pCommandList = CreateCommandList(pCommandAllocator);

    pCommandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), pCommandAllocator.get());

    m_commandListTable.emplace(threadID, CommandListEntry{ pCommandList, pCommandAllocator });

    return pCommandList;
}

std::shared_ptr<ID3D12CommandQueue> DrawingCommandManager_D3D12::GetCommandQueue() const
{
    return m_pCommandQueue;
}

uint64_t DrawingCommandManager_D3D12::ExecuteCommandLists()
{
    std::thread::id threadID = std::this_thread::get_id();
    auto it = m_commandListTable.find(threadID);
    if (it == m_commandListTable.cend())
        return 0;

    auto pCommandList = it->second.m_pCommandList;

    FlushBarriers();
    pCommandList->Close();

    std::vector<ID3D12CommandList*> ppCommandLists;

    auto pPendingCommandList = GetCommandList(true);
    bool bPendingBarrier = m_pResourceStateTracker->FlushPendingBarriers(pCommandList, pPendingCommandList);
    m_pResourceStateTracker->CommitFinalResourceStates(pCommandList);
    pPendingCommandList->Close();

    if (bPendingBarrier)
        ppCommandLists.push_back(pPendingCommandList.get());

    ppCommandLists.push_back(pCommandList.get());

    ID3D12CommandAllocator* pCommandAllocatorRaw = nullptr;
    ID3D12CommandAllocator* pPendingCommandAllocatorRaw = nullptr;

    m_pCommandQueue->ExecuteCommandLists(static_cast<UINT>(ppCommandLists.size()), ppCommandLists.data());
    uint64_t fenceValue = Signal();

    auto its = m_commandListTable.equal_range(it->first);
    for (auto i = its.first; i != its.second; ++i) {
        m_commandAllocatorInFlightQueue.Push(CommandAllocatorEntry{ fenceValue, std::move(i->second.m_pCommandAllocator) });
        m_commandListAwaitQueue.Push(std::move(i->second.m_pCommandList));
    }

    m_commandListTable.erase(it->first);

    return fenceValue;
}

uint64_t DrawingCommandManager_D3D12::Signal()
{
    uint64_t fenceValue = ++m_fenceValue;
    m_pCommandQueue->Signal(m_pFence.get(), fenceValue);
    return fenceValue;
}

bool DrawingCommandManager_D3D12::IsFenceComplete(uint64_t fenceValue)
{
    return m_pFence->GetCompletedValue() >= fenceValue;
}

void DrawingCommandManager_D3D12::WaitForFenceValue(uint64_t fenceValue)
{
    if (!IsFenceComplete(fenceValue))
    {
        m_pFence->SetEventOnCompletion(fenceValue, m_pFenceEvent);
        ::WaitForSingleObject(m_pFenceEvent, INFINITE);
    }
}

void DrawingCommandManager_D3D12::Flush()
{
    WaitForFenceValue(m_fenceValue);
}

void DrawingCommandManager_D3D12::TransitionBarrier(std::shared_ptr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateAfter, bool bForceFlush)
{
    m_pResourceStateTracker->TransitionBarrier(GetCommandList(), pResource, stateAfter);

    if (bForceFlush)
        FlushBarriers();
}

void DrawingCommandManager_D3D12::FlushBarriers()
{
    m_pResourceStateTracker->FlushBarriers(GetCommandList());
}

std::shared_ptr<ID3D12CommandAllocator> DrawingCommandManager_D3D12::CreateCommandAllocator()
{
    ID3D12CommandAllocator* pCommandAllocatorRaw;
    HRESULT hr = m_pDevice->GetDevice()->CreateCommandAllocator(D3D12Enum(m_type), __uuidof(ID3D12CommandAllocator), (void**)&pCommandAllocatorRaw);
    assert(SUCCEEDED(hr));
    auto pCommandAllocator = std::shared_ptr<ID3D12CommandAllocator>(pCommandAllocatorRaw, D3D12Releaser<ID3D12CommandAllocator>);

    return pCommandAllocator;
}

std::shared_ptr<ID3D12GraphicsCommandList> DrawingCommandManager_D3D12::CreateCommandList(std::shared_ptr<ID3D12CommandAllocator> allocator)
{
    ID3D12GraphicsCommandList* pCommandListRaw;
    HRESULT hr = m_pDevice->GetDevice()->CreateCommandList(0, D3D12Enum(m_type), allocator.get(), nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)&pCommandListRaw);
    assert(SUCCEEDED(hr));
    auto pCommandList = std::shared_ptr<ID3D12GraphicsCommandList>(pCommandListRaw, D3D12Releaser<ID3D12GraphicsCommandList>);

    return pCommandList;
}

void DrawingCommandManager_D3D12::CommandListInFlightProcess()
{
    while (m_isCommandListInFlightThreadRun)
    {
        CommandAllocatorEntry commandAllocatorEntry;

        while (m_commandAllocatorInFlightQueue.TryPop(commandAllocatorEntry))
        {
            WaitForFenceValue(commandAllocatorEntry.fenceValue);
            commandAllocatorEntry.m_pCommandAllocator->Reset();

            m_commandAllocatorAwaitQueue.Push(commandAllocatorEntry.m_pCommandAllocator);
        }

        std::this_thread::yield();
    }
}
