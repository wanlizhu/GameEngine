#include "DrawingDevice_D3D12.h"
#include "DrawingCommandManager_D3D12.h"

using namespace Engine;

DrawingCommandList_D3D12::DrawingCommandList_D3D12(const std::shared_ptr<DrawingDevice_D3D12> device, EDrawingCommandListType type) :
    m_pDevice(device), m_type(type), m_pCommandAllocator(nullptr), m_pCommandList(nullptr), m_pResourceStateTracker(nullptr)
{
    ID3D12CommandAllocator* pCommandAllocatorRaw = nullptr;
    HRESULT hr = m_pDevice->GetDevice()->CreateCommandAllocator(D3D12Enum(m_type), __uuidof(ID3D12CommandAllocator), (void**)&pCommandAllocatorRaw);
    assert(SUCCEEDED(hr));
    m_pCommandAllocator = std::shared_ptr<ID3D12CommandAllocator>(pCommandAllocatorRaw, D3D12Releaser<ID3D12CommandAllocator>);

    ID3D12GraphicsCommandList* pCommandListRaw = nullptr;
    hr = m_pDevice->GetDevice()->CreateCommandList(0, D3D12Enum(m_type), pCommandAllocatorRaw, nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)&pCommandListRaw);
    assert(SUCCEEDED(hr));
    m_pCommandList = std::shared_ptr<ID3D12GraphicsCommandList>(pCommandListRaw, D3D12Releaser<ID3D12GraphicsCommandList>);

    m_pResourceStateTracker = std::make_shared<DrawingResourceStateTracker_D3D12>();

    m_pUploadAllocator = std::make_shared<DrawingUploadAllocator_D3D12>(m_pDevice);

    for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        m_pDescriptorAllocators[i] = std::make_shared<DrawingDescriptorAllocator_D3D12>(m_pDevice, static_cast<EDrawingDescriptorHeapType>(i));
        m_pDynamicDescriptorHeaps[i] = std::make_shared<DrawingDynamicDescriptorHeap_D3D12>(m_pDevice, static_cast<EDrawingDescriptorHeapType>(i));
    }
}

DrawingCommandList_D3D12::~DrawingCommandList_D3D12()
{
}

EDrawingCommandListType DrawingCommandList_D3D12::GetCommandListType() const
{
    return m_type;
}

std::shared_ptr<ID3D12CommandAllocator> DrawingCommandList_D3D12::GetCommandAllocator() const
{
    return m_pCommandAllocator;
}

std::shared_ptr<ID3D12GraphicsCommandList> DrawingCommandList_D3D12::GetCommandList() const
{
    return m_pCommandList;
}

std::shared_ptr<DrawingUploadAllocator_D3D12> DrawingCommandList_D3D12::GetUploadAllocator() const
{
    return m_pUploadAllocator;
}

std::shared_ptr<DrawingDescriptorAllocator_D3D12> DrawingCommandList_D3D12::GetDescriptorAllocator(EDrawingDescriptorHeapType type) const
{
    return m_pDescriptorAllocators[type];
}

std::shared_ptr<DrawingDynamicDescriptorHeap_D3D12> DrawingCommandList_D3D12::GetDynamicDescriptorHeap(EDrawingDescriptorHeapType type) const
{
    return m_pDynamicDescriptorHeaps[type];
}

DrawingDescriptorAllocator_D3D12::Allocation DrawingCommandList_D3D12::AllocationDescriptors(EDrawingDescriptorHeapType type, uint32_t numDescriptors)
{
    return m_pDescriptorAllocators[type]->Allocate(numDescriptors);
}

void DrawingCommandList_D3D12::TransitionBarrier(std::shared_ptr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateAfter, bool bForceFlush)
{
    m_pResourceStateTracker->TransitionBarrier(shared_from_this(), pResource, stateAfter);

    if (bForceFlush)
        FlushBarriers();
}

void DrawingCommandList_D3D12::FlushBarriers()
{
    m_pResourceStateTracker->FlushBarriers(shared_from_this());
}

void DrawingCommandList_D3D12::Reset()
{
    HRESULT hr = m_pCommandAllocator->Reset();
    assert(SUCCEEDED(hr));

    hr = m_pCommandList->Reset(m_pCommandAllocator.get(), nullptr);
    assert(SUCCEEDED(hr));

    for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
        m_pDynamicDescriptorHeaps[i]->Reset();
}

bool DrawingCommandList_D3D12::Close(std::shared_ptr<DrawingCommandList_D3D12>& pPendingCommandList)
{
    FlushBarriers();
    m_pCommandList->Close();

    bool hasPendingBarriers = m_pResourceStateTracker->FlushPendingBarriers(shared_from_this(), pPendingCommandList);
    m_pResourceStateTracker->CommitFinalResourceStates(shared_from_this());

    return hasPendingBarriers;
}

void DrawingCommandList_D3D12::Close()
{
    FlushBarriers();
    m_pCommandList->Close();
}

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

    m_commandListInFlightThread = std::thread(&DrawingCommandManager_D3D12::CommandListInFlightProcess, this);
}

DrawingCommandManager_D3D12::~DrawingCommandManager_D3D12()
{
    m_isCommandListInFlightThreadRun = false;
}

std::shared_ptr<DrawingCommandList_D3D12> DrawingCommandManager_D3D12::GetCommandList(bool bForceNew)
{
    std::thread::id threadID = std::this_thread::get_id();
    auto it = m_commandListTable.find(threadID);
    if (it != m_commandListTable.cend() && !bForceNew)
        return it->second;

    std::shared_ptr<DrawingCommandList_D3D12> pCommandList = nullptr;

    if (!m_commandListAwaitQueue.Empty())
        m_commandListAwaitQueue.TryPop(pCommandList);
    else
        pCommandList = std::make_shared<DrawingCommandList_D3D12>(m_pDevice, m_type);

    m_commandListTable.emplace(threadID, pCommandList);

    return pCommandList;
}

std::shared_ptr<ID3D12CommandQueue> DrawingCommandManager_D3D12::GetCommandQueue() const
{
    return m_pCommandQueue;
}

uint64_t DrawingCommandManager_D3D12::ExecuteAllCommandLists()
{
    std::vector<std::shared_ptr<DrawingCommandList_D3D12>> pCommandListArray;
    std::thread::id threadID = std::this_thread::get_id();
    for (auto elem : m_commandListTable)
    {
        if (elem.first == threadID)
            pCommandListArray.emplace_back(elem.second);
    }

    uint64_t fenceValue = ExecuteCommandLists(pCommandListArray);
    m_commandListTable.erase(threadID);

    return fenceValue;
}

uint64_t DrawingCommandManager_D3D12::ExecuteCommandLists(const std::vector<std::shared_ptr<DrawingCommandList_D3D12>>& pCommandLists)
{
    std::vector<std::shared_ptr<DrawingCommandList_D3D12>> pCommandsInFlight;
    pCommandsInFlight.reserve(pCommandLists.size() * 2);

    std::vector<ID3D12CommandList*> pCommandListsRaw;
    pCommandListsRaw.reserve(pCommandLists.size() * 2);

    for (auto pCommandList : pCommandLists)
    {
        auto pPendingCommandList = GetCommandList(true);
        bool hasPendingBarriers = pCommandList->Close(pPendingCommandList);
        pPendingCommandList->Close();

        if (hasPendingBarriers)
            pCommandListsRaw.push_back(pPendingCommandList->GetCommandList().get());

        pCommandListsRaw.push_back(pCommandList->GetCommandList().get());

        pCommandsInFlight.emplace_back(pPendingCommandList);
        pCommandsInFlight.emplace_back(pCommandList);
    }

    auto num = static_cast<UINT>(pCommandListsRaw.size());

    m_pCommandQueue->ExecuteCommandLists(num, pCommandListsRaw.data());
    uint64_t fenceValue = Signal();

    for (auto pCommandList : pCommandsInFlight)
        m_commandListInFlightQueue.Push(CommandListEntry{ fenceValue, pCommandList });

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
        auto pFenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
        m_pFence->SetEventOnCompletion(fenceValue, pFenceEvent);
        ::WaitForSingleObject(pFenceEvent, INFINITE);
        ::CloseHandle(pFenceEvent);
    }
}

void DrawingCommandManager_D3D12::Flush()
{
    WaitForFenceValue(m_fenceValue);
}

void DrawingCommandManager_D3D12::CommandListInFlightProcess()
{
    std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

    while (m_isCommandListInFlightThreadRun)
    {
        CommandListEntry commandListEntry;

        lock.lock();
        while (m_commandListInFlightQueue.TryPop(commandListEntry))
        {
            auto fence = commandListEntry.m_fenceValue;
            auto pCommandList = commandListEntry.m_pCommandList;
            WaitForFenceValue(fence);
            pCommandList->Reset();

            m_commandListAwaitQueue.Push(pCommandList);
        }
        lock.unlock();

        std::this_thread::yield();
    }
}
