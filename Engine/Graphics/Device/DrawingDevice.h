
#pragma once

#include <memory>
#include <string>

#include "IDrawingSystem.h"

#include "CameraComponent.h"
#include "TransformComponent.h"

#include "DirtyData.h"
#include "DrawingType.h"
#include "DrawingParameter.h"
#include "DrawingRawResource.h"
#include "DrawingResourceDesc.h"
#include "DrawingResourceTable.h"
#include "DrawingParameter.h"

namespace Engine
{
    class DrawingDevice;
    class DrawingResourceTable;
    class DrawingResource
    {
    public:
        DrawingResource(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingResource()
        {
        }

        const std::shared_ptr<DrawingDevice>& GetDevice();

        virtual const std::shared_ptr<std::string>& GetName() const;
        virtual void SetName(std::shared_ptr<std::string> pName);
        virtual const std::shared_ptr<DrawingResourceDesc>& GetDesc() const;
        virtual void SetDesc(std::shared_ptr<DrawingResourceDesc> pDesc);

        virtual EDrawingResourceType GetType() const = 0;

    protected:
        std::shared_ptr<DrawingDevice> m_pDevice;
        std::shared_ptr<std::string> m_pName;
        std::shared_ptr<DrawingResourceDesc> m_pDesc;
    };

    template<typename T>
    class DrawingResourceWrapper : public DrawingResource
    {
    public:
        DrawingResourceWrapper(const std::shared_ptr<DrawingDevice>& pDevice) : DrawingResource(pDevice), m_pRes(nullptr) {}
        virtual ~DrawingResourceWrapper() { m_pRes = nullptr; }

        void SetResource(std::shared_ptr<T> pRes) { m_pRes = pRes; }
        const std::shared_ptr<T>& GetResource() { return m_pRes; }
 
    private:
        std::shared_ptr<T> m_pRes;
    };

    class DrawingTexture : public DrawingResourceWrapper<DrawingRawTexture>
    {
    public:
        DrawingTexture(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingTexture();

        EDrawingResourceType GetType() const override;
    };

    class DrawingShader : public DrawingResourceWrapper<DrawingRawShader>
    {
    public:
        DrawingShader(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingShader();
    };

    class DrawingVertexShader : public DrawingShader
    {
    public:
        using DrawingShader::DrawingShader;
        EDrawingResourceType GetType() const override;
    };

    class DrawingPixelShader : public DrawingShader
    {
    public:
        using DrawingShader::DrawingShader;
        EDrawingResourceType GetType() const override;
    };

    class DrawingTexBuffer : public DrawingResourceWrapper<DrawingRawTexBuffer>
    {
    public:
        DrawingTexBuffer(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingTexBuffer();

        EDrawingResourceType GetType() const override;
    };

    class DrawingRWBuffer : public DrawingResourceWrapper<DrawingRawRWBuffer>
    {
    public:
        DrawingRWBuffer(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingRWBuffer();

        EDrawingResourceType GetType() const override;
    };

    class DrawingEffect : public DrawingResourceWrapper<DrawingRawEffect>
    {
    public:
        DrawingEffect(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingEffect();

        EDrawingResourceType GetType() const override;
    };

    class DrawingBlendState : public DrawingResourceWrapper<DrawingRawBlendState>
    {
    public:
        DrawingBlendState(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingBlendState();

        EDrawingResourceType GetType() const override;
    };

    class DrawingDepthState : public DrawingResourceWrapper<DrawingRawDepthState>
    {
    public:
        DrawingDepthState(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingDepthState();

        EDrawingResourceType GetType() const override;
    };

    class DrawingRasterState : public DrawingResourceWrapper<DrawingRawRasterState>
    {
    public:
        DrawingRasterState(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingRasterState();

        EDrawingResourceType GetType() const override;
    };

    class DrawingSamplerState : public DrawingResourceWrapper<DrawingRawSamplerState>
    {
    public:
        DrawingSamplerState(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingSamplerState();

        EDrawingResourceType GetType() const override;
    };

    class DrawingVertexFormat : public DrawingResourceWrapper<DrawingRawVertexFormat>
    {
    public:
        DrawingVertexFormat(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingVertexFormat();

        EDrawingResourceType GetType() const override;
    };

    class DrawingVertexBuffer : public DrawingResourceWrapper<DrawingRawVertexBuffer>
    {
    public:
        DrawingVertexBuffer(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingVertexBuffer();

        EDrawingResourceType GetType() const override;
    };

    class DrawingIndexBuffer : public DrawingResourceWrapper<DrawingRawIndexBuffer>
    {
    public:
        DrawingIndexBuffer(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingIndexBuffer();

        EDrawingResourceType GetType() const override;
    };

    class DrawingConstantBuffer : public DrawingResource
    {
    public:
        DrawingConstantBuffer(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingConstantBuffer();

        void AddParameter(std::shared_ptr<DrawingParameter> pParam);
        void RemoveParameter(std::shared_ptr<DrawingParameter> pParam);
        std::shared_ptr<DrawingParameter> GetParameter(std::shared_ptr<std::string> pName);

        bool UpdateEffect(std::shared_ptr<DrawingEffect> pEffect);

        EDrawingResourceType GetType() const override;

    private:
        void ClearParameters();
        bool UpdateParameterToEffect(std::shared_ptr<DrawingParameter> pParam, std::shared_ptr<DrawingEffect> pEffect);

    private:
        std::shared_ptr<DrawingParameterSet> m_pParams;
    };

    class DrawingTarget : public DrawingResourceWrapper<DrawingRawTarget>
    {
    public:
        DrawingTarget(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingTarget();

        EDrawingResourceType GetType() const override;
    };

    class DrawingDepthBuffer : public DrawingResourceWrapper<DrawingRawTarget>
    {
    public:
        DrawingDepthBuffer(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingDepthBuffer();

        EDrawingResourceType GetType() const override;
    };

    class DrawingPrimitive : public DrawingResource
    {
    public:
        DrawingPrimitive(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingPrimitive();

        EDrawingPrimitiveType GetPrimitiveType() const;
        void SetPrimitiveType(EDrawingPrimitiveType type);

        uint32_t GetVertexCount() const;
        void SetVertexCount(uint32_t count);

        uint32_t GetIndexCount() const;
        void SetIndexCount(uint32_t count);

        uint32_t GetInstanceCount() const;
        void SetInstanceCount(uint32_t count);

        uint32_t GetVertexOffset() const;
        void SetVertexOffset(uint32_t offset);

        uint32_t GetIndexOffset() const;
        void SetIndexOffset(uint32_t offset);

        uint32_t GetInstanceOffset() const;
        void SetInstanceOffset(uint32_t offset);

        EDrawingResourceType GetType() const override;

    private:
        EDrawingPrimitiveType m_primitiveType;
        uint32_t m_vertexCount;
        uint32_t m_indexCount;
        uint32_t m_instanceCount;

        uint32_t m_vertexOffset;
        uint32_t m_indexOffset;
        uint32_t m_instanceOffset;
    };

    class DrawingVaringStates : public DrawingResource
    {
    public:
        DrawingVaringStates(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingVaringStates();

        void SetViewport(const Box2& vp);
        Box2 GetViewport() const;

        EDrawingResourceType GetType() const override;

    private:
        Box2 mViewport;
    };

    class DrawingPipelineState : public DrawingResourceWrapper<DrawingRawPipelineState>
    {
    public:
        DrawingPipelineState(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingPipelineState();

        EDrawingResourceType GetType() const override;

        typedef std::unordered_map<DrawingPipelineStateDesc::EPipelineStateSubobjectType, std::shared_ptr<DrawingResource>> SubobjectResourceTable;
    };

    class DrawingContext
    {
    public:
        DrawingContext(const std::shared_ptr<DrawingDevice>& pDevice);
        virtual ~DrawingContext();

        std::shared_ptr<DrawingNativeContext> GetNativeContext();
        void SetNativeContext(std::shared_ptr<DrawingNativeContext> context);

        void SetSwapChain(const std::shared_ptr<DrawingTarget> pSwapChain);
        std::shared_ptr<DrawingTarget> GetSwapChain() const;

        void SetDepthBuffer(const std::shared_ptr<DrawingDepthBuffer> pDepthBuffer);
        std::shared_ptr<DrawingDepthBuffer> GetDepthBuffer() const;

        void SetViewport(const Box2& vp);
        Box2 GetViewport() const;

        void UpdateContext(DrawingResourceTable& resTable);
        void UpdateTransform(DrawingResourceTable& resTable, float4x4 trans);
        void UpdateCamera(DrawingResourceTable& resTable, float4x4 proj, float4x4 view);

        void UpdateTargets(DrawingResourceTable& resTable);
        void UpdateViewport(DrawingResourceTable& resTable);

        std::shared_ptr<DrawingVaringStates> GetVaringStates(DrawingResourceTable& resTable);

    protected:
        std::shared_ptr<DrawingNativeContext> m_pNativeContext;
        std::shared_ptr<DrawingDevice> m_pDevice;

        uint2 m_screenSize;
        float2 m_invScreenSize;
        Box2 m_viewport;

        std::shared_ptr<DrawingTarget> m_pSwapChain;
        std::shared_ptr<DrawingDepthBuffer> m_pDepthBuffer;
    };

    class DrawingDevice : public std::enable_shared_from_this<DrawingDevice>
    {
    public:
        DrawingDevice() = default;
        virtual ~DrawingDevice() = default;

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual bool CreateVertexFormat(const DrawingVertexFormatDesc& desc, std::shared_ptr<DrawingVertexFormat>& pRes) = 0;
        virtual bool CreateVertexBuffer(const DrawingVertexBufferDesc& desc, std::shared_ptr<DrawingVertexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes, const void* pData = nullptr, uint32_t size = 0) = 0;
        virtual bool CreateIndexBuffer(const DrawingIndexBufferDesc& desc, std::shared_ptr<DrawingIndexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes, const void* pData = nullptr, uint32_t size = 0) =  0;
        virtual bool CreateTexture(const DrawingTextureDesc& desc, std::shared_ptr<DrawingTexture>& pRes, const void* pData = nullptr, uint32_t size = 0) = 0;
        virtual bool CreateTarget(const DrawingTargetDesc& desc, std::shared_ptr<DrawingTarget>& pRes) = 0;
        virtual bool CreateDepthBuffer(const DrawingDepthBufferDesc& desc, std::shared_ptr<DrawingDepthBuffer>& pRes) = 0;
        virtual bool CreateConstantBuffer(const DrawingConstantBufferDesc& desc, std::shared_ptr<DrawingConstantBuffer>& pRes);

        virtual bool CreateBlendState(const DrawingBlendStateDesc& desc, std::shared_ptr<DrawingBlendState>& pRes) = 0;
        virtual bool CreateDepthState(const DrawingDepthStateDesc& desc, std::shared_ptr<DrawingDepthState>& pRes) = 0;
        virtual bool CreateRasterState(const DrawingRasterStateDesc& desc, std::shared_ptr<DrawingRasterState>& pRes) = 0;
        virtual bool CreateSamplerState(const DrawingSamplerStateDesc& desc, std::shared_ptr<DrawingSamplerState>& pRes) = 0;

        virtual bool CreateEffectFromFile(const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes) = 0;
        virtual bool CreateEffectFromString(const std::string& str, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes) = 0;
        virtual bool CreateEffectFromBuffer(const void* pData, uint32_t length, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes) = 0;
        virtual bool CreateEffectFromShader(const DrawingEffectDesc& desc, std::shared_ptr<DrawingVertexShader> pVSShader, std::shared_ptr<DrawingPixelShader> pPSShader, std::shared_ptr<DrawingEffect>& pRes) = 0;

        virtual bool CreateVertexShaderFromFile(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes) = 0;
        virtual bool CreateVertexShaderFromString(const std::string& str, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes) = 0;
        virtual bool CreateVertexShaderFromBuffer(const void* pData, uint32_t length, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes) = 0;
        virtual bool CreatePixelShaderFromFile(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes) = 0;
        virtual bool CreatePixelShaderFromString(const std::string& str, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes) = 0;
        virtual bool CreatePixelShaderFromBuffer(const void* pData, uint32_t length, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes) = 0;

        virtual bool CreatePrimitive(const DrawingPrimitiveDesc& desc, std::shared_ptr<DrawingPrimitive>& pRes);
        virtual bool CreateVaringStates(const DrawingVaringStatesDesc& desc, std::shared_ptr<DrawingVaringStates>& pRes);

        virtual bool CreatePipelineState(const DrawingPipelineStateDesc& desc, const DrawingPipelineState::SubobjectResourceTable& subobjectResources, std::shared_ptr<DrawingPipelineState>& pRes) = 0;

        virtual void ClearTarget(std::shared_ptr<DrawingTarget> pTarget, const float4& color) = 0;
        virtual void ClearDepthBuffer(std::shared_ptr<DrawingDepthBuffer> pDepthBuffer, float depth, uint8_t stencil, uint32_t flag) = 0;

        virtual void SetVertexFormat(std::shared_ptr<DrawingVertexFormat> pFormat) = 0;
        virtual void SetVertexBuffer(std::shared_ptr<DrawingVertexBuffer> pVB[], uint32_t count) = 0;
        virtual void SetIndexBuffer(std::shared_ptr<DrawingIndexBuffer> pIB) = 0;

        virtual void SetBlendState(std::shared_ptr<DrawingBlendState> pBlend, float4 blendFactor, uint32_t sampleMask) = 0;
        virtual void SetDepthState(std::shared_ptr<DrawingDepthState> pDepth, uint32_t stencilRef) = 0;
        virtual void SetRasterState(std::shared_ptr<DrawingRasterState> pRaster) = 0;

        virtual void SetPipelineState(std::shared_ptr<DrawingPipelineState> pPipelineState) = 0;

        virtual void PushBlendState() = 0;
        virtual void PopBlendState() = 0;
        virtual void PushDepthState() = 0; 
        virtual void PopDepthState() = 0;
        virtual void PushRasterState() = 0;
        virtual void PopRasterState() = 0;

        virtual void SetViewport(Box2* vp) = 0;

        virtual void SetTargets(std::shared_ptr<DrawingTarget> pTarget[], uint32_t maxTargets, std::shared_ptr<DrawingDepthBuffer> pDepthBuffer, std::shared_ptr<DrawingRWBuffer> pRWBuffer[], uint32_t maxRWBuffers) = 0;

        virtual bool UpdateEffectParameter(std::shared_ptr<DrawingParameter> pParam, std::shared_ptr<DrawingEffect> pEffect) = 0;
        virtual bool UpdateEffectTexture(std::shared_ptr<DrawingTexture> pTex, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) = 0;
        virtual bool UpdateEffectBuffer(std::shared_ptr<DrawingTexBuffer> pBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) = 0;
        virtual bool UpdateEffectSampler(std::shared_ptr<DrawingSamplerState> pSampler, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) = 0;
        virtual bool UpdateEffectTexBuffer(std::shared_ptr<DrawingTexBuffer> pTexBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) = 0;
        virtual bool UpdateEffectRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) = 0;
        virtual bool UpdateEffectInputRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) = 0;
        virtual bool UpdateEffectOutputRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect) = 0;

        virtual void BeginEffect(DrawingContext& dc, std::shared_ptr<DrawingEffect> pEffect) = 0;
        virtual void EndEffect(DrawingContext& dc, std::shared_ptr<DrawingEffect> pEffect) = 0;

        virtual bool DrawPrimitive(std::shared_ptr<DrawingPrimitive> pRes) = 0;
        virtual bool Present(const std::shared_ptr<DrawingTarget> pTarget, uint32_t syncInterval) = 0;

        virtual void Flush() = 0;
    };

    template<EDeviceType type>
    static std::shared_ptr<DrawingDevice> CreateNativeDevice() { return nullptr; }
}