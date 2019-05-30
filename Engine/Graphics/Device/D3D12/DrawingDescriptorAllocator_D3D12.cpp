#include <d3dx12.h>

#include "DrawingDevice_D3D12.h"
#include "DrawingDescriptorAllocator_D3D12.h"

using namespace Engine;

DrawingDescriptorAllocator_D3D12::DrawingDescriptorAllocator_D3D12(const std::shared_ptr<DrawingDevice_D3D12> pDevice, EDrawingDescriptorHeapType type, uint32_t numDescriptorsPerPage) :
    m_pDevice(pDevice), m_type(type), m_numDescriptorsPerPage(numDescriptorsPerPage)
{
}

DrawingDescriptorAllocator_D3D12::~DrawingDescriptorAllocator_D3D12()
{
}

DrawingDescriptorAllocator_D3D12::Allocation DrawingDescriptorAllocator_D3D12::Allocate(uint32_t numDescriptors)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    Allocation allocation;
    for (auto it = m_availPages.begin(); it != m_availPages.end(); ++it)
    {
        auto curPage = m_pagePool[*it];
        allocation = curPage->Allocate(numDescriptors);
        if (curPage->GetNumFreeHandles() == 0)
            it = m_availPages.erase(it);

        if (allocation.IsValid())
            break;
    }

    if (!allocation.IsValid())
    {
        m_numDescriptorsPerPage = max(m_numDescriptorsPerPage, numDescriptors);
        auto newPage = GetNewPage();
        allocation = newPage->Allocate(numDescriptors);
        if (newPage->GetNumFreeHandles() == 0)
            m_availPages.erase(m_pagePool.size() - 1);

        assert(allocation.IsValid());
    }

    return allocation;
}

std::shared_ptr<DrawingDescriptorAllocator_D3D12::Page> DrawingDescriptorAllocator_D3D12::GetNewPage()
{
    auto newPage = std::make_shared<Page>(m_pDevice, m_type, m_numDescriptorsPerPage);
    m_pagePool.emplace_back(newPage);
    m_availPages.insert(m_pagePool.size() - 1);
    return newPage;
}

DrawingDescriptorAllocator_D3D12::Page::Page(const std::weak_ptr<DrawingDevice_D3D12> device, EDrawingDescriptorHeapType type, uint32_t numDescriptors) :
    m_pDevice(device), m_type(type), m_numDescriptorsInPage(numDescriptors)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12Enum(m_type);
    desc.NumDescriptors = m_numDescriptorsInPage;

    auto pDevice = m_pDevice.lock();
    assert(pDevice != nullptr);

    ID3D12DescriptorHeap* pDescriptorHeapRaw = nullptr;
    HRESULT hr = pDevice->GetDevice()->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), (void**)&pDescriptorHeapRaw);
    assert(SUCCEEDED(hr));

    m_pDescriptorHeap = std::shared_ptr<ID3D12DescriptorHeap>(pDescriptorHeapRaw, D3D12Releaser<ID3D12DescriptorHeap>);

    m_currentHandle = m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    m_descriptorHandleIncrementSize = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12Enum(m_type));
    m_numFreeHandles = m_numDescriptorsInPage;

    AddBlock(0, m_numDescriptorsInPage);
}

DrawingDescriptorAllocator_D3D12::Page::~Page()
{
}

bool DrawingDescriptorAllocator_D3D12::Page::HasEnoughSpace(uint32_t numDescriptors) const
{
    return m_blockListBySize.lower_bound(numDescriptors) != m_blockListBySize.end();
}

EDrawingDescriptorHeapType DrawingDescriptorAllocator_D3D12::Page::GetDescriptorHeap() const
{
    return m_type;
}

uint32_t DrawingDescriptorAllocator_D3D12::Page::GetNumFreeHandles() const
{
    return m_numFreeHandles;
}

DrawingDescriptorAllocator_D3D12::Allocation DrawingDescriptorAllocator_D3D12::Page::Allocate(uint32_t numDescriptors)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (numDescriptors > m_numFreeHandles)
        return Allocation();

    auto availableBlockIt = m_blockListBySize.lower_bound(numDescriptors);
    if (availableBlockIt == m_blockListBySize.end())
        return Allocation();

    auto blockSize = availableBlockIt->first;
    auto offsetIt = availableBlockIt->second;
    auto offset = offsetIt->first;

    m_blockListBySize.erase(availableBlockIt);
    m_blockListByOffset.erase(offsetIt);

    auto newSize = blockSize - numDescriptors;
    auto newOffset = offset + numDescriptors;

    if (newSize > 0)
        AddBlock(newOffset, newSize);

    m_numFreeHandles -= numDescriptors;
    return Allocation(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_currentHandle, offset, m_descriptorHandleIncrementSize), numDescriptors, m_descriptorHandleIncrementSize, shared_from_this());
}

void DrawingDescriptorAllocator_D3D12::Page::AddBlock(uint32_t offset, uint32_t numDescriptors)
{
    auto offsetIt = m_blockListByOffset.emplace(offset, numDescriptors);
    auto sizeIt = m_blockListBySize.emplace(numDescriptors, offsetIt.first);
    offsetIt.first->second.m_blockListBySizeIt = sizeIt; 
}