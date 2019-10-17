#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <d3d12shader.h>
#include <dxgi.h>
#include <algorithm>
#include <d3d12sdklayers.h>

#include <wrl.h>

#include "DrawingConstants.h"
#include "DrawingDevice_D3D12.h"
#include "DrawingRawResource.h"
#include "DrawingDynamicDescriptorHeap_D3D12.h"
#include "DrawingUtil_D3D12.h"

using namespace Microsoft::WRL;

namespace Engine
{
    class DrawingRawShader_D3D12 : public DrawingRawShader_Common
    {
    public:
        DrawingRawShader_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, std::shared_ptr<ID3D12ShaderReflection> pReflection, std::shared_ptr<ID3DBlob> pShaderBlob) : DrawingRawShader_Common(), m_pDevice(pDevice), m_pReflection(pReflection), m_pShaderBlob(pShaderBlob) {}
        virtual ~DrawingRawShader_D3D12() {}

        void BuildResourceBindingTable(DrawingRawShader::DrawingRawShaderType type);

        std::shared_ptr<ID3DBlob> GetBlob() const
        {
            return m_pShaderBlob;
        }

    private:
        void ProcessVariables(std::shared_ptr<std::string> pName, uint32_t size, ID3D12ShaderReflectionConstantBuffer* pBuffer, uint32_t count);

    protected:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        std::shared_ptr<ID3DBlob> m_pShaderBlob;
        std::shared_ptr<ID3D12ShaderReflection> m_pReflection;
    };

    class DrawingRawVertexShader_D3D12 : public DrawingRawVertexShader
    {
    public:
        DrawingRawVertexShader_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, std::shared_ptr<std::string> pShaderName, std::shared_ptr<ID3D12ShaderReflection> pReflection, std::shared_ptr<ID3DBlob> pShaderBlob) :
            DrawingRawVertexShader(pShaderName), m_pShaderImpl(std::make_shared<DrawingRawShader_D3D12>(pDevice, pReflection, pShaderBlob))
        {
            assert(m_pShaderImpl != nullptr);
            m_pShaderImpl->BuildResourceBindingTable(GetShaderType());
        }

        virtual ~DrawingRawVertexShader_D3D12() = default;

        std::shared_ptr<ID3DBlob> Get() const
        {
            return m_pShaderImpl->GetBlob();
        }

        DrawingRawShader_D3D12* GetShaderImpl() const
        {
            return m_pShaderImpl.get();
        }

    private:
        std::shared_ptr<DrawingRawShader_D3D12> m_pShaderImpl;
    };

    class DrawingRawPixelShader_D3D12 : public DrawingRawPixelShader
    {
    public:
        DrawingRawPixelShader_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, std::shared_ptr<std::string> pShaderName, std::shared_ptr<ID3D12ShaderReflection> pReflection, std::shared_ptr<ID3DBlob> pShaderBlob) :
            DrawingRawPixelShader(pShaderName), m_pShaderImpl(std::make_shared<DrawingRawShader_D3D12>(pDevice, pReflection, pShaderBlob))
        {
            assert(m_pShaderImpl != nullptr);
            m_pShaderImpl->BuildResourceBindingTable(GetShaderType());
        }

        virtual ~DrawingRawPixelShader_D3D12() = default;

        std::shared_ptr<ID3DBlob> Get() const
        {
            return m_pShaderImpl->GetBlob();
        }

        DrawingRawShader_D3D12* GetShaderImpl() const
        {
            return m_pShaderImpl.get();
        }

    private:
        std::shared_ptr<DrawingRawShader_D3D12> m_pShaderImpl;
    };

    class DrawingRawEffect_D3D12 : public DrawingRawEffect
    {
    public:
        DrawingRawEffect_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, std::shared_ptr<std::string> pEffectName) :
            DrawingRawEffect(pEffectName), m_pDevice(pDevice), m_pInputSignature(nullptr), m_byteCodeLength(0)
        {
            m_pRootSignature = std::make_shared<DrawingRootSignature_D3D12>(m_pDevice);
        }

        virtual ~DrawingRawEffect_D3D12() = default;

        const void* GetInputSignature() const
        {
            return m_pInputSignature;
        }

        uint64_t GetByteCodeLength() const
        {
            return m_byteCodeLength;
        }

        std::shared_ptr<DrawingRootSignature_D3D12> GetRootSignature()
        {
            return m_pRootSignature;
        }

    protected:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        const void* m_pInputSignature;
        uint64_t m_byteCodeLength;

        std::shared_ptr<DrawingRootSignature_D3D12> m_pRootSignature;
    };

    class DrawingRawShaderEffect_D3D12 : public DrawingRawEffect_D3D12
    {
    public:
        DrawingRawShaderEffect_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, std::shared_ptr<std::string> pEffectName, std::shared_ptr<DrawingRawVertexShader_D3D12> pVertexShader, std::shared_ptr<DrawingRawPixelShader_D3D12> pPixelShader);

        void Apply() override;
        void Terminate() override;

        std::shared_ptr<DrawingRawShader> GetRawShader(DrawingRawShader::DrawingRawShaderType type)
        {
            return m_shaderBlocks[type];
        }

    private:
        struct SConstBuffer
        {
            std::shared_ptr<std::string> mpName = nullptr;
            uint32_t mSizeInBytes = 0;

            std::array<uint32_t, DrawingRawShader::RawShader_Count> mStartSlot;
            std::shared_ptr<DrawingRawConstantBuffer> mpCB;
            SConstBuffer()
            {
                mStartSlot.fill(EMPTY_SLOT);
            }
        };

        struct SParamRes
        {
            std::shared_ptr<DrawingParameter> mpParam = nullptr;
            uint32_t mCount = 0;

            std::array<uint32_t, DrawingRawShader::RawShader_Count> mStartSlot;

            SParamRes()
            {
                mStartSlot.fill(EMPTY_SLOT);
            }
        };

        struct SParamVar
        {
            std::shared_ptr<DrawingParameter> mpParam = nullptr;
            uint32_t mSizeInBytes = 0;

            std::array<uint32_t, DrawingRawShader::RawShader_Count> mOffset;
            std::array<std::shared_ptr<DrawingRawConstantBuffer>, DrawingRawShader::RawShader_Count> mpCB;

            SParamVar()
            {
                mOffset.fill(0);
            }

            void UpdateValues(void);
        };
        void CheckAndAddResource(const DrawingRawShader_Common::ShaderResourceDesc& desc, uint32_t paramType, const DrawingRawShader::DrawingRawShaderType shaderType, std::unordered_map<std::shared_ptr<std::string>, SParamRes>& resTable) const;

        void LoadShaderInfo(const DrawingRawShader_D3D12* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);
        void LoadConstantBufferFromShader(const DrawingRawShader_D3D12* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);
        void LoadTexturesFromShader(const DrawingRawShader_D3D12* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);
        bool CreateRootSignature();

        void UpdateParameterValues();
        void UpdateConstantBuffers();
        void UpdateRootSignature();
        void UpdateDescriptor();
        void UpdateDevice();

        void BindConstantBuffer(DrawingDevice::ConstBufferPropTable& cbPropTable, const DrawingRawShader_D3D12* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);
        void GenerateParameters(const DrawingRawShader_D3D12* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);

        static constexpr const int32_t EMPTY_SLOT = -1;

        std::array<std::shared_ptr<DrawingRawShader>, DrawingRawShader::RawShader_Count> m_shaderBlocks;

        std::vector<CD3DX12_ROOT_PARAMETER1> m_rootParameters;

        std::unordered_map<std::shared_ptr<std::string>, SParamRes> mTexTable;
        std::unordered_map<std::shared_ptr<std::string>, SParamRes> mSamplerTable;
        std::unordered_map<std::shared_ptr<std::string>, SParamRes> mTexBufferTable;
        std::unordered_map<std::shared_ptr<std::string>, SParamRes> mRWBufferTable;
        std::unordered_map<std::shared_ptr<std::string>, SConstBuffer> mConstBufferTable;
        std::unordered_map<std::shared_ptr<std::string>, SParamVar> mVarTable;
    };

    class DrawingRawVertexFormat_D3D12 : public DrawingRawVertexFormat
    {
    public:
        DrawingRawVertexFormat_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, const std::vector<D3D12_INPUT_ELEMENT_DESC>& desc) : m_pDevice(pDevice)
        {
            std::for_each(desc.cbegin(), desc.cend(), [this](auto& elem)
            {
                m_inputDescs.emplace_back(elem);
            });

            m_inputLayout = D3D12_INPUT_LAYOUT_DESC { m_inputDescs.data(), (UINT)m_inputDescs.size() };
        }

        virtual ~DrawingRawVertexFormat_D3D12() = default;

        const D3D12_INPUT_LAYOUT_DESC& Get() const
        {
            return m_inputLayout;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputDescs;
        D3D12_INPUT_LAYOUT_DESC m_inputLayout;
    };

    class DrawingRawBuffer_D3D12
    {
    public:
        DrawingRawBuffer_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, uint32_t size, uint32_t stride) :
            m_pDevice(pDevice), m_sizeInBytes(size), m_stride(stride)
        {
        }

        virtual std::shared_ptr<ID3D12Resource> GetBuffer(void) const
        {
            return nullptr;
        }

        uint64_t GetSizeInBytes() const
        {
            return m_sizeInBytes;
        }

        void SetSizeInBytes(uint64_t size)
        {
            m_sizeInBytes = size;
        }

        uint64_t GetStride() const
        {
            return m_stride;
        }

        void SetStride(uint64_t stride)
        {
            m_stride = stride;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;

        uint64_t m_sizeInBytes;
        uint64_t m_stride;
    };

    class DrawingRawStaticBuffer_D3D12 : public DrawingRawBuffer_D3D12
    {
    public:
        DrawingRawStaticBuffer_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, D3D12_SUBRESOURCE_DATA& data, uint32_t size, uint32_t stride) :
            DrawingRawBuffer_D3D12(pDevice, size, stride)
        {
            auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

            HRESULT hr = pDevice->GetDevice()->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                __uuidof(ID3D12Resource),
                (void**)&m_pResource);

            assert(SUCCEEDED(hr));

            DrawingResourceStateTracker_D3D12::AddGlobalResourceState(m_pResource, D3D12_RESOURCE_STATE_COMMON);

            if (data.SlicePitch == 0)
                return;

            auto pCommandManager = pDevice->GetCommandManager(eCommandList_Copy);
            assert(pCommandManager != nullptr);

            auto pCommandList = pCommandManager->GetCommandList();
            assert(pCommandList != nullptr);

            pCommandList->TransitionBarrier(m_pResource, D3D12_RESOURCE_STATE_COPY_DEST);
            pCommandList->FlushBarriers();

            auto requiredSize = GetRequiredIntermediateSize(m_pResource.get(), 0, 1);
            auto uploadAllocation = pCommandList->AllocationUpload(requiredSize, stride);

            UpdateSubresources(pCommandList->GetCommandList().get(), m_pResource.get(), uploadAllocation.m_page->GetResource().get(), uploadAllocation.m_offset, 0, 1, &data);
        }

        std::shared_ptr<ID3D12Resource> GetBuffer(void) const override
        {
            return m_pResource;
        }

    private:
        std::shared_ptr<ID3D12Resource> m_pResource;
    };

    class DrawingRawDynamicBuffer_D3D12 : public DrawingRawBuffer_D3D12
    {
    public:
        DrawingRawDynamicBuffer_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, D3D12_SUBRESOURCE_DATA& data, uint32_t size, uint32_t stride) :
            DrawingRawBuffer_D3D12(pDevice, size, stride)
        {
            auto pCommandManager = pDevice->GetCommandManager(eCommandList_Copy);
            assert(pCommandManager != nullptr);

            auto pCommandList = pCommandManager->GetCommandList();
            assert(pCommandList != nullptr);

            m_uploadAllocation = pCommandList->AllocationUpload(size, stride);

            if (data.SlicePitch == 0)
                return;

            memcpy(m_uploadAllocation.m_pCPUData, data.pData, size);
        }

        DrawingUploadAllocator_D3D12::Allocation GetUploadAllocation() const
        {
            return m_uploadAllocation;
        }

    private:
        DrawingUploadAllocator_D3D12::Allocation m_uploadAllocation;
    };

    class DrawingRawVertexBuffer_D3D12 : public DrawingRawVertexBuffer
    {
    public:
        DrawingRawVertexBuffer_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, D3D12_SUBRESOURCE_DATA& data, uint32_t size, uint32_t stride, bool bStatic = false) :
            m_pDevice(pDevice)
        {
            if (bStatic)
            {
                m_pBufferImpl = std::make_shared<DrawingRawStaticBuffer_D3D12>(pDevice, data, size, stride);
                auto pStaticBufferImpl = std::dynamic_pointer_cast<DrawingRawStaticBuffer_D3D12>(m_pBufferImpl);
                assert(pStaticBufferImpl != nullptr);

                m_vertexBufferView.BufferLocation = pStaticBufferImpl->GetBuffer()->GetGPUVirtualAddress();
            }
            else
            {
                m_pBufferImpl = std::make_shared<DrawingRawDynamicBuffer_D3D12>(pDevice, data, size, stride);
                auto pDynamicBufferImpl = std::dynamic_pointer_cast<DrawingRawDynamicBuffer_D3D12>(m_pBufferImpl);
                assert(pDynamicBufferImpl != nullptr);

                m_vertexBufferView.BufferLocation = pDynamicBufferImpl->GetUploadAllocation().m_pGPUAddr;
            }

            m_vertexBufferView.SizeInBytes = static_cast<UINT>(size);
            m_vertexBufferView.StrideInBytes = static_cast<UINT>(stride);
        }

        virtual ~DrawingRawVertexBuffer_D3D12() = default;

        std::shared_ptr<ID3D12Resource> GetBuffer(void) const
        {
            return m_pBufferImpl->GetBuffer();
        }

        D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const
        {
            return m_vertexBufferView;
        }

        uint64_t GetSizeInBytes() const
        {
            return m_pBufferImpl->GetSizeInBytes();
        }

        uint64_t GetStride() const
        {
            return m_pBufferImpl->GetStride();
        }

    private:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        std::shared_ptr<DrawingRawBuffer_D3D12> m_pBufferImpl;

        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    };

    class DrawingRawIndexBuffer_D3D12 : public DrawingRawIndexBuffer
    {
    public:
        DrawingRawIndexBuffer_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, D3D12_SUBRESOURCE_DATA& data, uint32_t size, uint32_t stride, bool bStatic = false) :
            m_pDevice(pDevice)
        {
            m_format = (stride == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

            if (bStatic)
            {
                m_pBufferImpl = std::make_shared<DrawingRawStaticBuffer_D3D12>(pDevice, data, size, stride);
                auto pStaticBufferImpl = std::dynamic_pointer_cast<DrawingRawStaticBuffer_D3D12>(m_pBufferImpl);
                assert(pStaticBufferImpl != nullptr);

                m_indexBufferView.BufferLocation = pStaticBufferImpl->GetBuffer()->GetGPUVirtualAddress();
            }
            else
            {
                m_pBufferImpl = std::make_shared<DrawingRawDynamicBuffer_D3D12>(pDevice, data, size, stride);
                auto pDynamicBufferImpl = std::dynamic_pointer_cast<DrawingRawDynamicBuffer_D3D12>(m_pBufferImpl);
                assert(pDynamicBufferImpl != nullptr);

                m_indexBufferView.BufferLocation = pDynamicBufferImpl->GetUploadAllocation().m_pGPUAddr;
            }

            m_indexBufferView.SizeInBytes = static_cast<UINT>(size);
            m_indexBufferView.Format = GetFormat();
        }

        virtual ~DrawingRawIndexBuffer_D3D12() = default;

        std::shared_ptr<ID3D12Resource> GetBuffer(void) const
        {
            return m_pBufferImpl->GetBuffer();
        }

        D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const
        {
            return m_indexBufferView;
        }

        uint64_t GetSizeInBytes() const
        {
            return m_pBufferImpl->GetSizeInBytes();
        }

        uint64_t GetStride() const
        {
            return m_pBufferImpl->GetStride();
        }

        DXGI_FORMAT GetFormat() const
        {
            return m_format;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        std::shared_ptr<DrawingRawBuffer_D3D12> m_pBufferImpl;

        DXGI_FORMAT m_format;

        D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
    };

    class DrawingRawConstantBuffer_D3D12 : public DrawingRawConstantBuffer
    {
    public:
        DrawingRawConstantBuffer_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, uint32_t sizeInBytes, uint32_t rootParameterIndex) :
            DrawingRawConstantBuffer(sizeInBytes), m_pDevice(pDevice), m_rootParameterIndex(rootParameterIndex)
        {
            m_pData = new char[sizeInBytes];
            memset(m_pData, 0, sizeof(char) * sizeInBytes);
        }

        virtual ~DrawingRawConstantBuffer_D3D12() = default;

        void UpdateToHardware() override
        {
            assert(m_pDevice != nullptr);
            assert(m_pData != nullptr);

            auto pCommandManager = m_pDevice->GetCommandManager(eCommandList_Direct);
            auto pCommandList = pCommandManager->GetCommandList();

            assert(pCommandList != nullptr);

            auto heapAllocation = pCommandList->AllocationUpload(m_sizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
            memcpy(heapAllocation.m_pCPUData, m_pData, m_sizeInBytes);

            pCommandList->GetCommandList()->SetGraphicsRootConstantBufferView(m_rootParameterIndex, heapAllocation.m_pGPUAddr);
        }

    private:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        uint32_t m_rootParameterIndex;
    };

    class DrawingRawTexture_D3D12 : public DrawingRawTexture
    {
    public:
        DrawingRawTexture_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, const D3D12_RESOURCE_DESC& desc, std::vector<D3D12_SUBRESOURCE_DATA>& data, uint32_t dataSize) :
            m_pDevice(pDevice)
        {
            ID3D12Resource* pResourceRaw = nullptr;

            HRESULT hr = pDevice->GetDevice()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &desc,
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                __uuidof(ID3D12Resource),
                (void**)&pResourceRaw);

            assert(SUCCEEDED(hr));

            m_pResource = std::shared_ptr<ID3D12Resource>(pResourceRaw, D3D12Releaser<ID3D12Resource>);

            auto pCommandManager = m_pDevice->GetCommandManager(eCommandList_Copy);
            auto pCommandList = pCommandManager->GetCommandList();

            m_allocation = pCommandList->AllocationDescriptors(eDescriptorHeap_CBV_SRV_UVA);

            DrawingResourceStateTracker_D3D12::AddGlobalResourceState(m_pResource, D3D12_RESOURCE_STATE_COMMON);
            CopySubresource(data);
        }

        void CopySubresource(std::vector<D3D12_SUBRESOURCE_DATA>& data)
        {
            auto pCommandManager = m_pDevice->GetCommandManager(eCommandList_Copy);
            auto pCommandList = pCommandManager->GetCommandList();

            pCommandList->TransitionBarrier(m_pResource, D3D12_RESOURCE_STATE_COPY_DEST);
            pCommandList->FlushBarriers();

            auto requiredSize = GetRequiredIntermediateSize(m_pResource.get(), 0, (UINT)data.size());

            ID3D12Resource* pUploadResourceRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(requiredSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                __uuidof(ID3D12Resource),
                (void**)&pUploadResourceRaw);

            assert(SUCCEEDED(hr));
            UpdateSubresources(pCommandList->GetCommandList().get(), m_pResource.get(), pUploadResourceRaw, 0, 0, (UINT)data.size(), data.data());
        }

        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const
        {
            auto shaderResourceViewHandle = m_allocation.m_pCPUHandle;
            return shaderResourceViewHandle;
        }

        std::shared_ptr<ID3D12Resource> GetBuffer() const
        {
            return m_pResource;
        }

    protected:
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        std::shared_ptr<ID3D12Resource> m_pResource;
        DrawingDescriptorAllocator_D3D12::Allocation m_allocation;
    };

    class DrawingRawTexture2D_D3D12 : public DrawingRawTexture_D3D12
    {
    public:
        DrawingRawTexture2D_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, const D3D12_RESOURCE_DESC& desc, std::vector<D3D12_SUBRESOURCE_DATA>& data, uint32_t dataSize) :
            DrawingRawTexture_D3D12(pDevice, desc, data, dataSize)
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = desc.Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = desc.MipLevels;

            m_pDevice->GetDevice()->CreateShaderResourceView(m_pResource.get(), &srvDesc, m_allocation.m_pCPUHandle);
        }
    };

    class DrawingRawTarget_D3D12 : public DrawingRawTarget
    {
    public:
        DrawingRawTarget_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice) : m_pDevice(pDevice) {}
        virtual ~DrawingRawTarget_D3D12() = default;

        virtual std::shared_ptr<ID3D12Resource> GetTarget() const
        {
            return m_pTarget;
        }

        virtual ETargetType GetTargetType() const = 0;

    protected:
        DrawingDescriptorAllocator_D3D12::Allocation m_allocation;
        std::shared_ptr<DrawingDevice_D3D12> m_pDevice;
        std::shared_ptr<ID3D12Resource> m_pTarget;
    };

    class DrawingRawFragmentTarget_D3D12 : public DrawingRawTarget_D3D12
    {
    public:
        DrawingRawFragmentTarget_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice) : DrawingRawTarget_D3D12(pDevice), m_bufferIndex(0) {}
        virtual ~DrawingRawFragmentTarget_D3D12() = default;

        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const = 0;

        uint32_t GetCurrentIndex() const { return m_bufferIndex; }

    protected:
        uint32_t m_bufferIndex;
    };

    class DrawingRawSwapChain_D3D12 : public DrawingRawFragmentTarget_D3D12
    {
    public:
        DrawingRawSwapChain_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, DXGI_SWAP_CHAIN_DESC& desc) : DrawingRawFragmentTarget_D3D12(pDevice)
        {
            IDXGISwapChain* pDXGISwapChainRaw = nullptr;
            IDXGISwapChain3* pDXGISwapChain3Raw = nullptr;
            HRESULT hr = m_pDevice->GetDXGIFactory()->CreateSwapChain(m_pDevice->GetCommandManager(eCommandList_Direct)->GetCommandQueue().get(), &desc, &pDXGISwapChainRaw);
            assert(SUCCEEDED(hr));
            hr = pDXGISwapChainRaw->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&pDXGISwapChain3Raw);
            m_pDXGISwapChain = std::shared_ptr<IDXGISwapChain3>(pDXGISwapChain3Raw, D3D12Releaser<IDXGISwapChain>);

            auto pCommandManager = m_pDevice->GetCommandManager(eCommandList_Copy);
            auto pCommandList = pCommandManager->GetCommandList();
            m_allocation = pCommandList->AllocationDescriptors(eDescriptorHeap_RTV, BUFFER_COUNT);
    
            auto renderTargetViewHandle = m_allocation.m_pCPUHandle;
            auto renderTargetViewDescriptorSize = m_allocation.m_size;

            ID3D12Resource* pTargetRaw[BUFFER_COUNT] = { nullptr };
            for (uint32_t i = 0; i < BUFFER_COUNT; ++i)
            {
                hr = m_pDXGISwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&pTargetRaw[i]);
                assert(SUCCEEDED(hr));
                m_pTargetArray[i] = std::shared_ptr<ID3D12Resource>(pTargetRaw[i], D3D12Releaser<ID3D12Resource>);
            
                m_pDevice->GetDevice()->CreateRenderTargetView(pTargetRaw[i], nullptr, renderTargetViewHandle);
                renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
            }

            m_bufferIndex = m_pDXGISwapChain->GetCurrentBackBufferIndex();
            m_pTarget = m_pTargetArray[m_bufferIndex];
        }

        virtual ~DrawingRawSwapChain_D3D12() = default;

        std::shared_ptr<IDXGISwapChain3> GetSwapChain() const
        {
            return m_pDXGISwapChain;
        }

        ETargetType GetTargetType() const override
        {
            return eTarget_SwapChain;
        }

        std::shared_ptr<ID3D12Resource> GetTarget() const override
        {
            return m_pTarget;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const override
        {
            auto renderTargetViewHandle = m_allocation.m_pCPUHandle;
            auto renderTargetViewDescriptorSize = m_allocation.m_size;
            renderTargetViewHandle.ptr += (renderTargetViewDescriptorSize * m_bufferIndex);

            return renderTargetViewHandle;
        }

        HRESULT Present(uint32_t syncInterval)
        {
            if (m_pDXGISwapChain != nullptr)
            {
                HRESULT hr = m_pDXGISwapChain->Present(syncInterval, 0);
                m_bufferIndex = m_pDXGISwapChain->GetCurrentBackBufferIndex();
                m_pTarget = m_pTargetArray[m_bufferIndex];
                return hr;
            }

            return S_FALSE;
        }

    private:
        std::shared_ptr<IDXGISwapChain3> m_pDXGISwapChain;
        std::shared_ptr<ID3D12Resource> m_pTargetArray[BUFFER_COUNT];
    };

    class DrawingRawDepthTarget_D3D12 : public DrawingRawTarget_D3D12
    {
    public:
        DrawingRawDepthTarget_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, D3D12_DEPTH_STENCIL_VIEW_DESC desc, uint32_t width, uint32_t height) : DrawingRawTarget_D3D12(pDevice)
        {
            auto pCommandManager = m_pDevice->GetCommandManager(eCommandList_Copy);
            auto pCommandList = pCommandManager->GetCommandList();
            m_allocation = pCommandList->AllocationDescriptors(eDescriptorHeap_DSV);

            auto depthStencilViewHandle = m_allocation.m_pCPUHandle;

            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = desc.Format;
            clearValue.DepthStencil = { 1.0f, 0 };

            ID3D12Resource* pTargetRaw;
            HRESULT hr = m_pDevice->GetDevice()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Tex2D(desc.Format, width, height,
                1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &clearValue,
                __uuidof(ID3D12Resource),
                (void**)&pTargetRaw
            );

            assert(SUCCEEDED(hr));

            m_pTarget = std::shared_ptr<ID3D12Resource>(pTargetRaw, D3D12Releaser<ID3D12Resource>);
            m_pDevice->GetDevice()->CreateDepthStencilView(m_pTarget.get(), &desc, depthStencilViewHandle);
        }

        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const
        {
            auto depthStencilViewHandle = m_allocation.m_pCPUHandle;
            return depthStencilViewHandle;
        }

        ETargetType GetTargetType() const override
        {
            return eTarget_Depth;
        }

    };
}