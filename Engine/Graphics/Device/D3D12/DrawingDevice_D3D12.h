
#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <queue>
#include <unordered_map>

#include "SafeQueue.h"
#include "DrawingConstants.h"
#include "DrawingType.h"
#include "DrawingDevice.h"
#include "DrawingCommandManager_D3D12.h"
#include "DrawingUploadAllocator_D3D12.h"
#include "DrawingResourceStateTracker_D3D12.h"
#include "DrawingUtil_D3D12.h"

namespace Engine
{
    class DrawingDevice_D3D12;
    class DrawingRawVertexShader_D3D12;
    class DrawingRawPixelShader_D3D12;
    class DrawingRawConstantBuffer_D3D12;
    class DrawingDevice_D3D12 : public DrawingDevice
    {
    public:
        const static uint32_t MAX_RENDER_TARGET_COUNT = 8;
        const static uint32_t MAX_UAV_SLOT_COUNT = 8;

        DrawingDevice_D3D12(const std::shared_ptr<ID3D12Device2> device);
        virtual ~DrawingDevice_D3D12();

        void Initialize() override;
        void Shutdown() override;

        bool CreateVertexFormat(const DrawingVertexFormatDesc& desc, std::shared_ptr<DrawingVertexFormat>& pRes) override;
        bool CreateVertexBuffer(const DrawingVertexBufferDesc& desc, std::shared_ptr<DrawingVertexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes, const void* pData = nullptr, uint32_t size = 0) override;
        bool CreateIndexBuffer(const DrawingIndexBufferDesc& desc, std::shared_ptr<DrawingIndexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes, const void* pData = nullptr, uint32_t size = 0) override;
        bool CreateTexture(const DrawingTextureDesc& desc, std::shared_ptr<DrawingTexture>& pRes, const void* pData = nullptr, uint32_t size = 0) override;
        bool CreateTarget(const DrawingTargetDesc& desc, std::shared_ptr<DrawingTarget>& pRes) override;
        bool CreateDepthBuffer(const DrawingDepthBufferDesc& desc, std::shared_ptr<DrawingDepthBuffer>& pRes) override;

        bool CreateBlendState(const DrawingBlendStateDesc& desc, std::shared_ptr<DrawingBlendState>& pRes) override;
        bool CreateDepthState(const DrawingDepthStateDesc& desc, std::shared_ptr<DrawingDepthState>& pRes) override;
        bool CreateRasterState(const DrawingRasterStateDesc& desc, std::shared_ptr<DrawingRasterState>& pRes) override;
        bool CreateSamplerState(const DrawingSamplerStateDesc& desc, std::shared_ptr<DrawingSamplerState>& pRes) override;

        bool CreateEffectFromFile(const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes) override;
        bool CreateEffectFromString(const std::string& str, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes) override;
        bool CreateEffectFromBuffer(const void* pData, uint32_t length, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes) override;
        bool CreateEffectFromShader(const DrawingEffectDesc& desc, std::shared_ptr<DrawingVertexShader> pVSShader, std::shared_ptr<DrawingPixelShader> pPSShader, std::shared_ptr<DrawingEffect>& pRes) override;

        bool CreateVertexShaderFromFile(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes) override;
        bool CreateVertexShaderFromString(const std::string& str, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes) override;
        bool CreateVertexShaderFromBuffer(const void* pData, uint32_t length, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes) override;
        bool CreatePixelShaderFromFile(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes) override;
        bool CreatePixelShaderFromString(const std::string& str, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes) override;
        bool CreatePixelShaderFromBuffer(const void* pData, uint32_t length, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes) override;

        bool CreatePipelineState(const DrawingPipelineStateDesc& desc, const DrawingPipelineState::SubobjectResourceTable& subobjectResources, std::shared_ptr<DrawingPipelineState>& pRes) override;

        void ClearTarget(std::shared_ptr<DrawingTarget> pTarget, const float4& color) override;
        void ClearDepthBuffer(std::shared_ptr<DrawingDepthBuffer> pDepthBuffer, float depth, uint8_t stencil, uint32_t flag) override;

        void SetVertexFormat(std::shared_ptr<DrawingVertexFormat> pFormat) override;
        void SetVertexBuffer(std::shared_ptr<DrawingVertexBuffer> pVB[], uint32_t count) override;
        void SetIndexBuffer(std::shared_ptr<DrawingIndexBuffer> pIB) override;

        void SetBlendState(std::shared_ptr<DrawingBlendState> pBlend, float4 blendFactor, uint32_t sampleMask) override;
        void SetDepthState(std::shared_ptr<DrawingDepthState> pDepth, uint32_t stencilRef) override;
        void SetRasterState(std::shared_ptr<DrawingRasterState> pRaster) override;

        void SetPipelineState(std::shared_ptr<DrawingPipelineState> pPipelineState) override;

        void PushBlendState() override;
        void PopBlendState() override;
        void PushDepthState() override;
        void PopDepthState() override;
        void PushRasterState() override;
        void PopRasterState() override;

        void SetViewport(Box2* vp) override;

        void SetTargets(std::shared_ptr<DrawingTarget> pTarget[], uint32_t maxTargets, std::shared_ptr<DrawingDepthBuffer> pDepthBuffer, std::shared_ptr<DrawingRWBuffer> pRWBuffer[], uint32_t maxRWBuffers) override;

        bool UpdateEffectParameter(std::shared_ptr<DrawingParameter> pParam, std::shared_ptr<DrawingEffect> pEffect) override;
        bool UpdateEffectTexture(std::shared_ptr<DrawingTexture> pTex, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) override;
        bool UpdateEffectBuffer(std::shared_ptr<DrawingTexBuffer> pBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) override;
        bool UpdateEffectSampler(std::shared_ptr<DrawingSamplerState> pSampler, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) override;
        bool UpdateEffectTexBuffer(std::shared_ptr<DrawingTexBuffer> pTexBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) override;
        bool UpdateEffectRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) override;
        bool UpdateEffectInputRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) override;
        bool UpdateEffectOutputRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) override;

        void BeginEffect(DrawingContext& dc, std::shared_ptr<DrawingEffect> pEffect) override;
        void EndEffect(DrawingContext& dc, std::shared_ptr<DrawingEffect> pEffect) override;

        bool DrawPrimitive(std::shared_ptr<DrawingPrimitive> pRes) override;
        bool Present(const std::shared_ptr<DrawingTarget> pTarget, uint32_t syncInterval) override;

        void Flush() override;

        std::shared_ptr<ID3D12Device2> GetDevice() const;
        std::shared_ptr<IDXGIFactory4> GetDXGIFactory() const;

        std::shared_ptr<DrawingCommandManager_D3D12> GetCommandManager(EDrawingCommandListType type) const;
        std::shared_ptr<DrawingUploadAllocator_D3D12> GetUploadAllocator() const;

    private:
        bool DoCreateEffect(const DrawingEffectDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingEffect>& pRes);

        bool DoCreateVertexShader(const DrawingVertexShaderDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingVertexShader>& pRes);
        bool DoCreatePixelShader(const DrawingPixelShaderDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingPixelShader>& pRes);

        // std::shared_ptr<DrawingRawFxEffect_D3D12> CreateEffectFromMemory(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pTechName, const void* pData, uint32_t size);

        std::shared_ptr<DrawingRawVertexShader_D3D12> CreateVertexShaderFromBuffer(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const void* pData, uint32_t size);
        std::shared_ptr<DrawingRawPixelShader_D3D12> CreatePixelShaderFromBuffer(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const void* pData, uint32_t size);

        std::shared_ptr<DrawingRawVertexShader_D3D12> CreateVertexShaderFromBlob(std::shared_ptr<std::string> pName, const void* pShaderByteCode, uint32_t length);
        std::shared_ptr<DrawingRawPixelShader_D3D12> CreatePixelShaderFromBlob(std::shared_ptr<std::string> pName, const void* pShaderByteCode, uint32_t length);

        std::shared_ptr<DrawingRawVertexShader_D3D12> CreateVertexShaderFromString(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const char* pSrc, uint32_t size);
        std::shared_ptr<DrawingRawPixelShader_D3D12> CreatePixelShaderFromString(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const char* pSrc, uint32_t size);

    private:
        std::shared_ptr<ID3D12Device2> m_pDevice;
        std::shared_ptr<IDXGIFactory4> m_pDXGIFactory;

        std::shared_ptr<DrawingCommandManager_D3D12> m_pDirectCommandManager;
        std::shared_ptr<DrawingCommandManager_D3D12> m_pComputeCommandManager;
        std::shared_ptr<DrawingCommandManager_D3D12> m_pCopyCommandManager;

        std::shared_ptr<DrawingUploadAllocator_D3D12> m_pUploadAllocator;

        uint64_t m_fenceValues[BUFFER_COUNT] = {};
    };

    template<>
    static std::shared_ptr<DrawingDevice> CreateNativeDevice<eDevice_D3D12>()
    {
#if defined(_DEBUG)
        ID3D12Debug1* pDebugInterface;
        D3D12GetDebugInterface(__uuidof(ID3D12Debug1), (void**)&pDebugInterface);
        pDebugInterface->EnableDebugLayer();
#endif

        uint32_t adaptorIndex = 0;
        ID3D12Device2* pDevice = nullptr;
        IDXGIFactory4* pDxgiFactory = nullptr;
        IDXGIAdapter4* pAdapter4 = nullptr; 
        IDXGIAdapter1* pAdapter1 = nullptr;
        std::vector<IDXGIAdapter4*> adapterList;

        uint32_t createFactoryFlags = 0;
#if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        HRESULT hr = CreateDXGIFactory2(createFactoryFlags, __uuidof(IDXGIFactory4), (void**)&pDxgiFactory);
        if (FAILED(hr))
            return nullptr;

        while (pDxgiFactory->EnumAdapters1(adaptorIndex, &pAdapter1) != DXGI_ERROR_NOT_FOUND)
        {
            pAdapter1->QueryInterface(&pAdapter4);
            hr = D3D12CreateDevice(pAdapter4, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device2), (void**)&pDevice);
            if (SUCCEEDED(hr))
            {
                std::shared_ptr<ID3D12Device2> pDevicePtr(pDevice, D3D12Releaser<ID3D12Device2>);
                return std::make_shared<DrawingDevice_D3D12>(pDevicePtr);
            }
            adapterList.push_back(pAdapter4);
            ++adaptorIndex;
        }

        return nullptr;
    }
}