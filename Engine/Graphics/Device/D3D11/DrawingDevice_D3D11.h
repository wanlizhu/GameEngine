#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <memory>
#include <stack>
#include <vector>

#include "Vector.h"
#include "DrawingDevice.h"
#include "DrawingUtil_D3D11.h"

namespace Engine
{
    class DrawingRawBlendState_D3D11;
    class DrawingRawDepthState_D3D11;
    class DrawingRawRasterState_D3D11;
    class DrawingRawVertexShader_D3D11;
    class DrawingRawPixelShader_D3D11;
    class DrawingRawFxEffect_D3D11;
    class DrawingRawConstantBuffer_D3D11;
    class DrawingDevice_D3D11 : public DrawingDevice
    {
    public:
        const static uint32_t MAX_RENDER_TARGET_COUNT = 8;
        const static uint32_t MAX_UAV_SLOT_COUNT = 8;

        typedef std::tuple<float4, uint32_t, std::shared_ptr<DrawingRawBlendState_D3D11>> BlendState;
        typedef std::tuple<uint32_t, std::shared_ptr<DrawingRawDepthState_D3D11>> DepthState;
        typedef std::tuple<std::shared_ptr<DrawingRawRasterState_D3D11>> RasterState;

        DrawingDevice_D3D11(const std::shared_ptr<ID3D11Device> device);
        virtual ~DrawingDevice_D3D11();

        void Initialize() override;
        void Shutdown() override;

        bool CreateVertexFormat(const DrawingVertexFormatDesc& desc, std::shared_ptr<DrawingVertexFormat>& pRes) override;
        bool CreateVertexBuffer(const DrawingVertexBufferDesc& desc, std::shared_ptr<DrawingVertexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes = nullptr, const void* pData = nullptr, uint32_t size = 0) override;
        bool CreateIndexBuffer(const DrawingIndexBufferDesc& desc, std::shared_ptr<DrawingIndexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes = nullptr, const void* pData = nullptr, uint32_t size = 0) override;
        bool CreateTexture(const DrawingTextureDesc& desc, std::shared_ptr<DrawingTexture>& pRes, const void* pData[] = nullptr, uint32_t size[] = nullptr, uint32_t slices = 0) override;
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

        void SetBlendState(const std::shared_ptr<DrawingRawBlendState_D3D11> pBlendState = nullptr, const float4& blendFactor = float4(1.0f), uint32_t sampleMask = 0xffffffff);
        void SetDepthState(const std::shared_ptr<DrawingRawDepthState_D3D11> pDepthState = nullptr, uint32_t stencilRef = 1);
        void SetRasterState(const std::shared_ptr<DrawingRawRasterState_D3D11> pRasterState = nullptr);

        void SetBlendState(const BlendState& state);
        void SetDepthState(const DepthState& state);
        void SetRasterState(const RasterState& state);

        BlendState GetBlendState();
        DepthState GetDepthState();
        RasterState GetRasterState();

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

        void* Map(std::shared_ptr<DrawingResource> pRes, uint32_t subID, EDrawingAccessType flag, uint32_t& rowPitch, uint32_t& slicePitch, uint32_t offset = 0, uint32_t sizeInBytes = 0) override;
        void UnMap(std::shared_ptr<DrawingResource> pRes, uint32_t subID) override;

        void Flush() override;

        uint32_t FormatBytes(EDrawingFormatType type) override;

        std::shared_ptr<ID3D11Device> GetDevice() const;
        std::shared_ptr<ID3D11DeviceContext> GetDeviceContext() const;
        std::shared_ptr<IDXGIFactory> GetDXGIFactory() const;

    private:
        bool DoCreateEffect(const DrawingEffectDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingEffect>& pRes);

        bool DoCreateVertexShader(const DrawingVertexShaderDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingVertexShader>& pRes);
        bool DoCreatePixelShader(const DrawingPixelShaderDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingPixelShader>& pRes);

        std::shared_ptr<DrawingRawFxEffect_D3D11> CreateEffectFromMemory(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pTechName, const void* pData, uint32_t size);

        std::shared_ptr<DrawingRawVertexShader_D3D11> CreateVertexShaderFromBuffer(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const void* pData, uint32_t size);
        std::shared_ptr<DrawingRawPixelShader_D3D11> CreatePixelShaderFromBuffer(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const void* pData, uint32_t size);

        std::shared_ptr<DrawingRawVertexShader_D3D11> CreateVertexShaderFromBlob(std::shared_ptr<std::string> pName, const void* pShaderByteCode, uint32_t length);
        std::shared_ptr<DrawingRawPixelShader_D3D11> CreatePixelShaderFromBlob(std::shared_ptr<std::string> pName, const void* pShaderByteCode, uint32_t length);

        std::shared_ptr<DrawingRawVertexShader_D3D11> CreateVertexShaderFromString(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const char* pSrc, uint32_t size);
        std::shared_ptr<DrawingRawPixelShader_D3D11> CreatePixelShaderFromString(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const char* pSrc, uint32_t size);

        template<typename T, typename U>
        void* MapResource(std::shared_ptr<DrawingResource> pRes, uint32_t subID, EDrawingAccessType flag, uint32_t& rowPitch, uint32_t& slicePitch);

        template<typename T, typename U>
        void UnMapResource(std::shared_ptr<DrawingResource> pRes, uint32_t aSubID);

    private:
        std::shared_ptr<ID3D11Device> m_pDevice;
        std::shared_ptr<ID3D11DeviceContext> m_pDeviceContext;
        std::shared_ptr<IDXGIDevice> m_pDXGIDevice;
        std::shared_ptr<IDXGIAdapter> m_pDXGIAdapter;
        std::shared_ptr<IDXGIFactory> m_pDXGIFactory;

        std::stack<BlendState> m_blendStates;
        std::stack<DepthState> m_depthStates;
        std::stack<RasterState> m_rasterStates;
    };

    template<>
    static std::shared_ptr<DrawingDevice> CreateNativeDevice<eDevice_D3D11>()
    {
        ID3D11Device* pDevice = nullptr;
        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        uint32_t flags = 0;

#ifdef _DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT hr = D3D11CreateDevice(
            0,
            D3D_DRIVER_TYPE_HARDWARE,
            0,
            flags,
            &featureLevel,
            1,
            D3D11_SDK_VERSION,
            &pDevice,
            nullptr,
            nullptr
            );



        if (FAILED(hr))
            return nullptr;

        std::shared_ptr<ID3D11Device> pDevicePtr(pDevice, D3D11Releaser<ID3D11Device>);
        return std::make_shared<DrawingDevice_D3D11>(pDevicePtr);
    }
}