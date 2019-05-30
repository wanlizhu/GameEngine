#include <d3dx12.h>

#include "DrawingDevice_D3D12.h"
#include "DrawingDynamicDescriptorHeap_D3D12.h"

using namespace Engine;

DrawingRootSignature_D3D12::DrawingRootSignature_D3D12(const std::shared_ptr<DrawingDevice_D3D12> pDevice) :
    m_pDevice(pDevice),
    m_rootSignatureDesc{},
    m_pRootSignature(nullptr),
    m_numDescriptorsPerTable{0},
    m_samplerTableBitMask(0),
    m_descriptorTableBitMask(0)
{
}

DrawingRootSignature_D3D12::DrawingRootSignature_D3D12(const std::shared_ptr<DrawingDevice_D3D12> pDevice, const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion) :
    m_pDevice(pDevice),
    m_rootSignatureDesc{},
    m_pRootSignature(nullptr),
    m_numDescriptorsPerTable{0},
    m_samplerTableBitMask(0),
    m_descriptorTableBitMask(0)
{
    SetRootSignatureDesc(rootSignatureDesc, rootSignatureVersion);
}

DrawingRootSignature_D3D12::~DrawingRootSignature_D3D12()
{
    Shutdown();
}

void DrawingRootSignature_D3D12::SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion)
{
    Shutdown();

    auto numParameters = rootSignatureDesc.NumParameters;
    D3D12_ROOT_PARAMETER1* pParameters = numParameters > 0 ? new D3D12_ROOT_PARAMETER1[numParameters] : nullptr;

    for (uint32_t i = 0; i < numParameters; ++i)
    {
        const D3D12_ROOT_PARAMETER1& rootParameter = rootSignatureDesc.pParameters[i];
        pParameters[i] = rootParameter;

        if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            uint32_t numDescriptorRanges = rootParameter.DescriptorTable.NumDescriptorRanges;
            D3D12_DESCRIPTOR_RANGE1* pDescriptorRanges = numDescriptorRanges > 0 ? new D3D12_DESCRIPTOR_RANGE1[numDescriptorRanges] : nullptr;

            memcpy(pDescriptorRanges, rootParameter.DescriptorTable.pDescriptorRanges, sizeof(D3D12_DESCRIPTOR_RANGE1) * numDescriptorRanges);

            pParameters[i].DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
            pParameters[i].DescriptorTable.pDescriptorRanges = pDescriptorRanges;

            if (numDescriptorRanges > 0)
                switch (pDescriptorRanges[0].RangeType)
                {
                    case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
                    case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                    case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
                        m_descriptorTableBitMask |= (1 << i);
                        break;
                    case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
                        m_samplerTableBitMask |= (1 << i);
                        break;
                }

            for (uint32_t j = 0; j < numDescriptorRanges; ++j)
                m_numDescriptorsPerTable[i] += pDescriptorRanges[j].NumDescriptors;
        }
    }

    m_rootSignatureDesc.NumParameters = numParameters;
    m_rootSignatureDesc.pParameters = pParameters;

    uint32_t numStaticSamplers = rootSignatureDesc.NumStaticSamplers;
    D3D12_STATIC_SAMPLER_DESC* pStaticSamplers = numStaticSamplers > 0 ? new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers] : nullptr;

    if (pStaticSamplers)
        memcpy( pStaticSamplers, rootSignatureDesc.pStaticSamplers, sizeof( D3D12_STATIC_SAMPLER_DESC ) * numStaticSamplers );

    m_rootSignatureDesc.NumStaticSamplers = numStaticSamplers;
    m_rootSignatureDesc.pStaticSamplers = pStaticSamplers;

    D3D12_ROOT_SIGNATURE_FLAGS flags = rootSignatureDesc.Flags;
    m_rootSignatureDesc.Flags = flags;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionRootSignatureDesc;
    versionRootSignatureDesc.Init_1_1(numParameters, pParameters, numStaticSamplers, pStaticSamplers, flags);

    ID3DBlob* rootSignatureBlob;
    ID3DBlob* errorBlob;
    HRESULT hr = D3DX12SerializeVersionedRootSignature(&versionRootSignatureDesc, rootSignatureVersion, &rootSignatureBlob, &errorBlob);
    assert(SUCCEEDED(hr));

    ID3D12RootSignature* pRootSignatureRaw = nullptr;
    hr = m_pDevice->GetDevice()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pRootSignatureRaw);
    m_pRootSignature = std::shared_ptr<ID3D12RootSignature>(pRootSignatureRaw, D3D12Releaser<ID3D12RootSignature>);
    assert(SUCCEEDED(hr));
}

void DrawingRootSignature_D3D12::Shutdown()
{
    for (uint32_t i = 0; i < m_rootSignatureDesc.NumParameters; ++i)
    {
        const D3D12_ROOT_PARAMETER1& rootParameter = m_rootSignatureDesc.pParameters[i];
        if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
            delete[] rootParameter.DescriptorTable.pDescriptorRanges;
    }

    delete[] m_rootSignatureDesc.pParameters; 
    m_rootSignatureDesc.pParameters = nullptr;
    m_rootSignatureDesc.NumParameters = 0;
    
    delete[] m_rootSignatureDesc.pStaticSamplers; 
    m_rootSignatureDesc.pStaticSamplers = nullptr;
    m_rootSignatureDesc.NumStaticSamplers = 0;

    m_descriptorTableBitMask = 0;
    m_samplerTableBitMask = 0;

    memset(m_numDescriptorsPerTable, 0, sizeof(m_numDescriptorsPerTable));
}

std::shared_ptr<ID3D12RootSignature> DrawingRootSignature_D3D12::GetRootSignature() const
{
    return m_pRootSignature;
}

const D3D12_ROOT_SIGNATURE_DESC1& DrawingRootSignature_D3D12::GetRootSignatureDesc() const
{
    return m_rootSignatureDesc;
}

uint32_t DrawingRootSignature_D3D12::GetDescriptorTableBitMask(EDrawingDescriptorHeapType type) const
{
    switch (type)
    {
        case eDescriptorHeap_CBV_SRV_UVA:
            return m_descriptorTableBitMask;
        case eDescriptorHeap_Sampler:
            return m_samplerTableBitMask;
    }
    return 0;
}

uint32_t DrawingRootSignature_D3D12::GetNumDescriptors(uint32_t index) const
{
    assert(index < 32);
    return m_numDescriptorsPerTable[index];
}

DrawingDynamicDescriptorHeap_D3D12::DrawingDynamicDescriptorHeap_D3D12(const std::shared_ptr<DrawingDevice_D3D12> pDevice, EDrawingDescriptorHeapType type, uint32_t numDescriptorsPerPage) :
    m_pDevice(pDevice),
    m_type(type),
    m_numDescriptorsPerPage(numDescriptorsPerPage),
    m_descriptorTableBitMask(0),
    m_staleDescriptorTableBitMask(0)
{
    m_descriptorHandleIncrementSize = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12Enum(m_type));
    m_currentGPUDescriptorHandle.ptr = 0;
    m_currentCPUDescriptorHandle.ptr = 0;

    m_pDescriptorHandleCache = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(m_numDescriptorsPerPage);
}

DrawingDynamicDescriptorHeap_D3D12::~DrawingDynamicDescriptorHeap_D3D12()
{
}

void DrawingDynamicDescriptorHeap_D3D12::StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors)
{
    assert((numDescriptors <= m_numDescriptorsPerPage) && (rootParameterIndex < MAX_DESCRIPTOR_TABLES));
    auto& descriptorTableCache = m_descriptorTableCaches[rootParameterIndex];

    assert((offset + numDescriptors) <= descriptorTableCache.m_numDescriptors);

    D3D12_CPU_DESCRIPTOR_HANDLE* dstDescriptor = (descriptorTableCache.m_pBaseDescriptor + offset);

    for (uint32_t i = 0; i < numDescriptors; ++i)
        dstDescriptor[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srcDescriptors, i, m_descriptorHandleIncrementSize);

    m_staleDescriptorTableBitMask |= (1 << rootParameterIndex);
}

void DrawingDynamicDescriptorHeap_D3D12::CommitStagedDescriptors(std::function<void(ID3D12GraphicsCommandList*, uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc)
{
    uint32_t numDescriptorsToCommit = ComputeStaleDescriptorCount();

    if (numDescriptorsToCommit > 0)
    {
        auto pCommandManager = m_pDevice->GetCommandManager(eCommandList_Direct);
        auto pCommandList = pCommandManager->GetCommandList();

        if (!m_pCurrentDescriptorHeap || m_numFreeHandles < numDescriptorsToCommit)
        {
            m_pCurrentDescriptorHeap = RequestDescriptorHeap();
            m_currentCPUDescriptorHandle = m_pCurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            m_currentGPUDescriptorHandle = m_pCurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
            m_numFreeHandles = m_numDescriptorsPerPage;

            m_pDevice->SetDescriptorHeap(m_type, m_pCurrentDescriptorHeap);
            m_staleDescriptorTableBitMask = m_descriptorTableBitMask;
        }

        DWORD rootIndex;
        while (_BitScanForward(&rootIndex, m_staleDescriptorTableBitMask))
        {
            UINT numSrcDescriptors = m_descriptorTableCaches[rootIndex].m_numDescriptors;
            D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorHandles = m_descriptorTableCaches[rootIndex].m_pBaseDescriptor;

            D3D12_CPU_DESCRIPTOR_HANDLE pDestDescriptorRangeStarts[] =
            {
                m_currentCPUDescriptorHandle
            };

            UINT pDestDescriptorRangeSizes[] =
            {
                numSrcDescriptors
            };

            m_pDevice->GetDevice()->CopyDescriptors(1, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, numSrcDescriptors, pSrcDescriptorHandles, nullptr, D3D12Enum(m_type));
            setFunc(pCommandList.get(), rootIndex, m_currentGPUDescriptorHandle);

            m_currentCPUDescriptorHandle.ptr += numSrcDescriptors * m_descriptorHandleIncrementSize;
            m_currentGPUDescriptorHandle.ptr += numSrcDescriptors * m_descriptorHandleIncrementSize;
            m_numFreeHandles -= numSrcDescriptors;

            m_staleDescriptorTableBitMask ^= (1 << rootIndex);
        }
    }
}

void DrawingDynamicDescriptorHeap_D3D12::CommitStagedDescriptorsForDraw()
{
    CommitStagedDescriptors(&ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
}

void DrawingDynamicDescriptorHeap_D3D12::CommitStagedDescriptorsForDispatch()
{
    CommitStagedDescriptors(&ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
}

void DrawingDynamicDescriptorHeap_D3D12::ParseRootSignature(const DrawingRootSignature_D3D12& rootSignature)
{
    m_staleDescriptorTableBitMask = 0;
    const auto& rootSignatureDesc = rootSignature.GetRootSignatureDesc();

    m_descriptorTableBitMask = rootSignature.GetDescriptorTableBitMask(m_type);
    uint32_t descriptorTableBitMask = m_descriptorTableBitMask;

    uint32_t currentOffset = 0;
    DWORD rootIndex;
    while (_BitScanForward(&rootIndex, descriptorTableBitMask) && rootIndex < rootSignatureDesc.NumParameters)
    {
        uint32_t numDescriptors = rootSignature.GetNumDescriptors(rootIndex);

        auto& descriptorTableCache = m_descriptorTableCaches[rootIndex];
        descriptorTableCache.m_numDescriptors = numDescriptors;
        descriptorTableCache.m_pBaseDescriptor = m_pDescriptorHandleCache.get() + currentOffset;

        currentOffset += numDescriptors;
        descriptorTableBitMask ^= (1 << rootIndex);
    }
}

void DrawingDynamicDescriptorHeap_D3D12::Reset()
{
    m_availableDescriptorHeaps = m_descriptorHeapPool;
    m_pCurrentDescriptorHeap = nullptr;
    m_currentCPUDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
    m_currentGPUDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
    m_numFreeHandles = 0;
    m_descriptorTableBitMask = 0;
    m_staleDescriptorTableBitMask = 0;

    for (int i = 0; i < MAX_DESCRIPTOR_TABLES; ++i)
        m_descriptorTableCaches[i].Reset();
}

std::shared_ptr<ID3D12DescriptorHeap> DrawingDynamicDescriptorHeap_D3D12::RequestDescriptorHeap()
{
    std::shared_ptr<ID3D12DescriptorHeap> pDescriptorHeap = nullptr;
    if (!m_availableDescriptorHeaps.empty())
    {
        pDescriptorHeap = m_availableDescriptorHeaps.front();
        m_availableDescriptorHeaps.pop();
    }
    else
    {
        pDescriptorHeap = CreateDescriptorHeap();
        m_descriptorHeapPool.push(pDescriptorHeap);
    }

    return pDescriptorHeap;
}

std::shared_ptr<ID3D12DescriptorHeap> DrawingDynamicDescriptorHeap_D3D12::CreateDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
    descriptorHeapDesc.Type = D3D12Enum(m_type);
    descriptorHeapDesc.NumDescriptors = m_numDescriptorsPerPage;
    descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ID3D12DescriptorHeap* pDescriptorHeapRaw;
    HRESULT hr = m_pDevice->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&pDescriptorHeapRaw);
    assert(SUCCEEDED(hr));

    auto pDescriptorHeap = std::shared_ptr<ID3D12DescriptorHeap>(pDescriptorHeapRaw, D3D12Releaser<ID3D12DescriptorHeap>);
    return pDescriptorHeap;
}

uint32_t DrawingDynamicDescriptorHeap_D3D12::ComputeStaleDescriptorCount() const
{
    uint32_t numStaleDescriptors = 0;
    DWORD i;
    DWORD staleDescriptorsBitMask = m_staleDescriptorTableBitMask;

    while ( _BitScanForward( &i, staleDescriptorsBitMask ) )
    {
        numStaleDescriptors += m_descriptorTableCaches[i].m_numDescriptors;
        staleDescriptorsBitMask ^= ( 1 << i );
    }

    return numStaleDescriptors;
}