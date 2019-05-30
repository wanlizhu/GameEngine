#pragma once

#include <memory>
#include <queue>
#include <functional>
#include <d3d12.h>

namespace Engine
{
    class DrawingDevice_D3D12;
    class DrawingRootSignature_D3D12
    {
    public:
        DrawingRootSignature_D3D12(const std::shared_ptr<DrawingDevice_D3D12> pDevice);
        DrawingRootSignature_D3D12(const std::shared_ptr<DrawingDevice_D3D12> pDevice, const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion);
        virtual ~DrawingRootSignature_D3D12();

        void SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion);
        void Shutdown();

        std::shared_ptr<ID3D12RootSignature> GetRootSignature() const;
        const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const;
        uint32_t GetDescriptorTableBitMask(EDrawingDescriptorHeapType type) const;
        uint32_t GetNumDescriptors(uint32_t index) const;

    private:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        D3D12_ROOT_SIGNATURE_DESC1 m_rootSignatureDesc;
        std::shared_ptr<ID3D12RootSignature> m_pRootSignature;

        uint32_t m_numDescriptorsPerTable[32];
        uint32_t m_samplerTableBitMask;
        uint32_t m_descriptorTableBitMask;
    };

    class DrawingDynamicDescriptorHeap_D3D12
    {
    public:
        DrawingDynamicDescriptorHeap_D3D12(const std::shared_ptr<DrawingDevice_D3D12> pDevice, EDrawingDescriptorHeapType type, uint32_t numDescriptorsPerPage = 1024);
        virtual ~DrawingDynamicDescriptorHeap_D3D12();

        void StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors);

        void CommitStagedDescriptors(std::function<void(ID3D12GraphicsCommandList*, uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc);
        void CommitStagedDescriptorsForDraw();
        void CommitStagedDescriptorsForDispatch();

        void ParseRootSignature(const DrawingRootSignature_D3D12& rootSignature);
        void Reset();

    private:
        std::shared_ptr<ID3D12DescriptorHeap> RequestDescriptorHeap();
        std::shared_ptr<ID3D12DescriptorHeap> CreateDescriptorHeap();
        uint32_t ComputeStaleDescriptorCount() const;

        static const uint32_t MAX_DESCRIPTOR_TABLES = 32;
        struct DescriptorTableCache
        {
            DescriptorTableCache() : 
                m_numDescriptors(0),
                m_pBaseDescriptor(nullptr) {}

            void Reset()
            {
                m_numDescriptors = 0;
                m_pBaseDescriptor = nullptr;
            }
            uint32_t m_numDescriptors;
            D3D12_CPU_DESCRIPTOR_HANDLE* m_pBaseDescriptor;
        };

        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        EDrawingDescriptorHeapType m_type;

        uint32_t m_numDescriptorsPerPage;
        uint32_t m_descriptorHandleIncrementSize;

        std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> m_pDescriptorHandleCache;
        DescriptorTableCache m_descriptorTableCaches[MAX_DESCRIPTOR_TABLES];

        uint32_t m_descriptorTableBitMask;
        uint32_t m_staleDescriptorTableBitMask;

        typedef std::queue<std::shared_ptr<ID3D12DescriptorHeap>> DescriptorHeapPool;

        DescriptorHeapPool m_descriptorHeapPool;
        DescriptorHeapPool m_availableDescriptorHeaps;

        std::shared_ptr<ID3D12DescriptorHeap> m_pCurrentDescriptorHeap;
        D3D12_GPU_DESCRIPTOR_HANDLE m_currentGPUDescriptorHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE m_currentCPUDescriptorHandle;

        uint32_t m_numFreeHandles;
    };
}