#include <d3dx12.h>

#include "Algorithm.h"

#include "DrawingDevice_D3D12.h"
#include "DrawingUtil_D3D12.h"
#include "DrawingUploadAllocator_D3D12.h"

using namespace Engine;

DrawingUploadAllocator_D3D12::DrawingUploadAllocator_D3D12(const std::shared_ptr<DrawingDevice_D3D12> pDevice, uint64_t pageSizeInBytes) :
    m_pDevice(pDevice), m_pageSizeInBytes(pageSizeInBytes)
{
}

DrawingUploadAllocator_D3D12::~DrawingUploadAllocator_D3D12()
{
}

DrawingUploadAllocator_D3D12::Allocation DrawingUploadAllocator_D3D12::Allocate(uint64_t sizeInBytes, uint64_t alignment)
{
    assert(sizeInBytes <= m_pageSizeInBytes);

    if (!m_pPageCur || !m_pPageCur->HasEnoughSpace(sizeInBytes, alignment))
        m_pPageCur = GetNewPage();

    return m_pPageCur->Allocate(sizeInBytes, alignment);
}

void DrawingUploadAllocator_D3D12::Reset()
{
    m_pPageCur = nullptr;
    m_pagePoolAvail = m_pagePool;

    for (auto pPage : m_pagePoolAvail)
        pPage->Reset();
}

std::shared_ptr<DrawingUploadAllocator_D3D12::Page> DrawingUploadAllocator_D3D12::GetNewPage()
{
    std::shared_ptr<Page> pPage;

    if (!m_pagePoolAvail.empty())
    {
        pPage = m_pagePoolAvail.front();
        m_pagePoolAvail.pop_front();
    }
    else
    {
        pPage = std::make_shared<Page>(m_pDevice, m_pageSizeInBytes);
        m_pagePool.push_back(pPage);
    }

    return pPage;
}

DrawingUploadAllocator_D3D12::Page::Page(const std::weak_ptr<DrawingDevice_D3D12> device, uint64_t sizeInBytes) :
    m_pDevice(device),
    m_startPtr(),
    m_offset(0),
    m_sizeInBytes(sizeInBytes)
{
    auto pDevice = m_pDevice.lock();
    assert(pDevice != nullptr);

    ID3D12Resource* pResourceRaw = nullptr;

    HRESULT hr = pDevice->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(m_sizeInBytes),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        __uuidof(ID3D12Resource),
        (void**)&pResourceRaw);

    assert(SUCCEEDED(hr));

    m_pResource = std::shared_ptr<ID3D12Resource>(pResourceRaw, D3D12Releaser<ID3D12Resource>);

    void* pCPUData = nullptr;
    m_pResource->Map(0, nullptr, &pCPUData);
    m_startPtr.m_pCPUData = pCPUData;
    m_startPtr.m_pGPUAddr = m_pResource->GetGPUVirtualAddress();
}

DrawingUploadAllocator_D3D12::Page::~Page()
{
    m_pResource->Unmap(0, nullptr);
    m_startPtr.m_pCPUData = nullptr;
    m_startPtr.m_pGPUAddr = D3D12_GPU_VIRTUAL_ADDRESS(0);
}

bool DrawingUploadAllocator_D3D12::Page::HasEnoughSpace(uint64_t sizeInBytes, uint64_t alignment)
{
    uint64_t alignedSize = AlignUp(sizeInBytes, alignment);
    uint64_t alignedOffset = AlignUp(m_offset, alignment);

    return alignedOffset + alignedSize <= m_sizeInBytes;
}

DrawingUploadAllocator_D3D12::Allocation DrawingUploadAllocator_D3D12::Page::Allocate(uint64_t sizeInBytes, uint64_t alignment)
{
    assert(HasEnoughSpace(sizeInBytes, alignment));

    std::lock_guard<std::mutex> lock(m_mutex);
    uint64_t alignedSize = AlignUp(sizeInBytes, alignment);
    m_offset = AlignUp(m_offset, alignment);

    Allocation allocation;
    allocation.m_pCPUData = static_cast<uint8_t*>(m_startPtr.m_pCPUData) + m_offset;
    allocation.m_pGPUAddr = m_startPtr.m_pGPUAddr + m_offset;

    m_offset += alignedSize;

    return allocation;
}

void DrawingUploadAllocator_D3D12::Page::Reset()
{
    m_offset = 0;
}