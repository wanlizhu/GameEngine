
 #pragma once

#include <stdint.h>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>

#include "DrawingConstants.h"

namespace Engine
{
    class DrawingResourceDesc
    {
    public:
        DrawingResourceDesc() = default;
        DrawingResourceDesc(const DrawingResourceDesc& desc);
        DrawingResourceDesc(DrawingResourceDesc&& desc);
        virtual ~DrawingResourceDesc();

        DrawingResourceDesc& operator= (const DrawingResourceDesc& rhs);

        virtual EDrawingResourceType GetType() const = 0;
        virtual DrawingResourceDesc* Clone() const = 0;

        typedef std::unordered_map<uint32_t, std::shared_ptr<std::string>> ResourceDescNamesType;

        ResourceDescNamesType GetResourceDescNames() const;
        void AddResourceDescName(uint32_t index, std::shared_ptr<std::string> pName);
        std::shared_ptr<std::string> GetResourceDescName(uint32_t index) const;

        bool IsExternalResource() const;  
        void SetIsExternalResource(bool flag);

    private:
        void CloneFromNames(const ResourceDescNamesType& from);

        ResourceDescNamesType m_resourceDescNames;
        bool m_externalRes;
    };

    class DrawingProgramDesc : public DrawingResourceDesc
    {
    public:
        DrawingProgramDesc();
        DrawingProgramDesc(const DrawingProgramDesc& desc);
        DrawingProgramDesc(DrawingProgramDesc&& desc);
        virtual ~DrawingProgramDesc();

        DrawingProgramDesc& operator= (const DrawingProgramDesc& rhs);

    public:
        EDrawingProgramType mProgramType;
        std::shared_ptr<std::string> mpName;
        std::shared_ptr<std::string> mpSourceName;
    };

    class DrawingShaderDesc : public DrawingProgramDesc
    {
    public:
        DrawingShaderDesc();
        DrawingShaderDesc(const DrawingShaderDesc& desc);
        DrawingShaderDesc(DrawingShaderDesc&& desc);
        virtual ~DrawingShaderDesc();

        DrawingShaderDesc& operator= (const DrawingShaderDesc& rhs);

    public:
        std::shared_ptr<std::string> mpEntryName;
        std::shared_ptr<std::string> mpIncludePath;
    };

    class DrawingVertexShaderDesc : public DrawingShaderDesc
    {
    public:
        DrawingVertexShaderDesc();
        DrawingVertexShaderDesc(const DrawingVertexShaderDesc& desc);
        DrawingVertexShaderDesc(DrawingVertexShaderDesc&& desc);
        virtual ~DrawingVertexShaderDesc();

        DrawingVertexShaderDesc& operator= (const DrawingVertexShaderDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;
    };

    class DrawingPixelShaderDesc : public DrawingShaderDesc
    {
    public:
        DrawingPixelShaderDesc();
        DrawingPixelShaderDesc(const DrawingPixelShaderDesc& desc);
        DrawingPixelShaderDesc(DrawingPixelShaderDesc&& desc);
        virtual ~DrawingPixelShaderDesc();

        DrawingPixelShaderDesc& operator= (const DrawingPixelShaderDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;
    };

    class DrawingEffectDesc : public DrawingProgramDesc
    {
    public:
        enum EffectType
        {
            EffectType_General,
            EffectType_Compute,
            EffectType_Linked,
        };

        DrawingEffectDesc();
        DrawingEffectDesc(const DrawingEffectDesc& desc);
        DrawingEffectDesc(DrawingEffectDesc&& desc);
        virtual ~DrawingEffectDesc();

        DrawingEffectDesc& operator= (const DrawingEffectDesc& rhs);

        virtual EffectType GetEffectType() const = 0;
        EDrawingResourceType GetType() const override;
    };

    class DrawingGeneralEffectDesc : public DrawingEffectDesc
    {
    public:
        DrawingGeneralEffectDesc();
        DrawingGeneralEffectDesc(const DrawingGeneralEffectDesc& desc);
        DrawingGeneralEffectDesc(DrawingGeneralEffectDesc&& desc);
        virtual ~DrawingGeneralEffectDesc();

        DrawingGeneralEffectDesc& operator= (const DrawingGeneralEffectDesc& rhs);

        EffectType GetEffectType() const override;
        DrawingResourceDesc* Clone() const override;

    public:
        std::shared_ptr<std::string> mpTechName;
    };

    class DrawingComputeEffectDesc : public DrawingEffectDesc
    {
    public:
        DrawingComputeEffectDesc();
        DrawingComputeEffectDesc(const DrawingComputeEffectDesc& desc);
        DrawingComputeEffectDesc(DrawingComputeEffectDesc&& desc);
        virtual ~DrawingComputeEffectDesc();

        DrawingComputeEffectDesc& operator= (const DrawingComputeEffectDesc& rhs);

        EffectType GetEffectType() const override;
        DrawingResourceDesc* Clone() const override;
    };

    class DrawingLinkedEffectDesc : public DrawingEffectDesc
    {
    public:
        DrawingLinkedEffectDesc();
        DrawingLinkedEffectDesc(const DrawingLinkedEffectDesc& desc);
        DrawingLinkedEffectDesc(DrawingLinkedEffectDesc&& desc);
        virtual ~DrawingLinkedEffectDesc();

        DrawingLinkedEffectDesc& operator= (const DrawingLinkedEffectDesc& rhs);

        EffectType GetEffectType() const override;
        DrawingResourceDesc* Clone() const override;

        static const uint32_t VERTEX_SHADER_ID = 0;
        static const uint32_t PIXEL_SHADER_ID = 1;
    };

    class DrawingVertexFormatDesc : public DrawingResourceDesc
    {
    public:
        DrawingVertexFormatDesc();
        DrawingVertexFormatDesc(const DrawingVertexFormatDesc& desc);
        DrawingVertexFormatDesc(DrawingVertexFormatDesc&& desc);
        virtual ~DrawingVertexFormatDesc();

        DrawingVertexFormatDesc& operator= (const DrawingVertexFormatDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;

    public:
        struct VertexInputElement
        {
            VertexInputElement();
            VertexInputElement(const VertexInputElement& elem);
            VertexInputElement(VertexInputElement&& elem);
            ~VertexInputElement();

            VertexInputElement& operator= (const VertexInputElement& rhs);

            std::shared_ptr<std::string> mpName;
            uint32_t mIndex; 
            EDrawingFormatType mFormat;
            uint32_t mSlot;
            uint32_t mOffset;
            uint32_t mInstanceStepRate; // This value needs to be 0 for per-vertex data.
                                        // Other value for per-instance data
        };

        std::vector<VertexInputElement> m_inputElements;
    };

    class DrawingBufferDesc : public DrawingResourceDesc
    {
    public:
        DrawingBufferDesc();
        DrawingBufferDesc(const DrawingBufferDesc& desc);
        DrawingBufferDesc(DrawingBufferDesc&& desc);
        virtual ~DrawingBufferDesc();

        DrawingBufferDesc& operator= (const DrawingBufferDesc& rhs);

    public:
        uint32_t mSizeInBytes;
        uint32_t mStrideInBytes;
        EDrawingUsageType mUsage;
        EDrawingAccessType mAccess;
        uint32_t mFlags;
    };

    class DrawingVertexBufferDesc : public DrawingBufferDesc
    {
    public:
        DrawingVertexBufferDesc();
        DrawingVertexBufferDesc(const DrawingVertexBufferDesc& desc);
        DrawingVertexBufferDesc(DrawingVertexBufferDesc&& desc);
        virtual ~DrawingVertexBufferDesc();

        DrawingVertexBufferDesc& operator= (const DrawingVertexBufferDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;
    };

    class DrawingIndexBufferDesc : public DrawingBufferDesc
    {
    public:
        DrawingIndexBufferDesc();
        DrawingIndexBufferDesc(const DrawingIndexBufferDesc& desc);
        DrawingIndexBufferDesc(DrawingIndexBufferDesc&& desc);
        virtual ~DrawingIndexBufferDesc();

        DrawingIndexBufferDesc& operator= (const DrawingIndexBufferDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;
    };

    class DrawingBlendStateDesc : public DrawingResourceDesc
    {
    public:
        DrawingBlendStateDesc();
        DrawingBlendStateDesc(const DrawingBlendStateDesc& desc);
        DrawingBlendStateDesc(DrawingBlendStateDesc&& desc);
        virtual ~DrawingBlendStateDesc();

        DrawingBlendStateDesc& operator= (const DrawingBlendStateDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;

        struct BlendDef
        {
            BlendDef();
            BlendDef(const BlendDef& blend);
            BlendDef(BlendDef&& blend);
            virtual ~BlendDef();

            BlendDef& operator= (const BlendDef& rhs);

            bool operator== (const BlendDef& rhs) const;
            bool operator!= (const BlendDef& rhs) const;

            EDrawingBlendType mBlendSrc;
            EDrawingBlendType mBlendDst;
            EDrawingBlendOpType mBlendOp;
        };
        struct BlendTarget
        {
            enum WriteMask
            {
                WriteMast_All = 0xf,
            };
            BlendTarget();
            BlendTarget(const BlendTarget& target);
            BlendTarget(BlendTarget&& target);
            virtual ~BlendTarget();

            BlendTarget& operator= (const BlendTarget& rhs);

            bool operator== (const BlendTarget& rhs) const;
            bool operator!= (const BlendTarget& rhs) const;

            bool mBlendEnable;
            BlendDef mAlphaBlend;
            BlendDef mColorBlend;
            uint8_t mRenderTargetWriteMask;
        };

    public:
        bool mAlphaToCoverageEnable;
        bool mIndependentBlendEnable;
        BlendTarget mTargets[MAX_TARGETS];
    };

    class DrawingDepthStateDesc : public DrawingResourceDesc
    {
    public:
        DrawingDepthStateDesc();
        DrawingDepthStateDesc(const DrawingDepthStateDesc& depth);
        DrawingDepthStateDesc(DrawingDepthStateDesc&& depth);
        virtual ~DrawingDepthStateDesc();

        DrawingDepthStateDesc& operator= (const DrawingDepthStateDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;

        struct DepthState
        {
            DepthState();
            DepthState(const DepthState& depth);
            DepthState(DepthState&& depth);
            virtual ~DepthState();

            DepthState& operator= (const DepthState& rhs);

            bool operator== (const DepthState& rhs) const;
            bool operator!= (const DepthState& rhs) const;

            bool mDepthEnable;
            bool mDepthWriteEnable;
            EDrawingComparisonFuncType mDepthFunc;
        };

        struct StencilOp
        {
            StencilOp();
            StencilOp(const StencilOp& op);
            StencilOp(StencilOp&& op);
            virtual ~StencilOp();

            StencilOp& operator= (const StencilOp& rhs);

            bool operator== (const StencilOp& rhs) const;
            bool operator!= (const StencilOp& rhs) const;

            EDrawingStencilOpType mStencilFailOp;
            EDrawingStencilOpType mStencilDepthFailOp;
            EDrawingStencilOpType mStencilPassOp;
            EDrawingComparisonFuncType mStencilFunc;
        };

        struct StencilState
        {
            enum Mask
            {
                Mast_All = 0xf,
            };
            StencilState();
            StencilState(const StencilState& stencil);
            StencilState(StencilState&& stencil);
            virtual ~StencilState();

            StencilState& operator= (const StencilState& rhs);

            bool operator== (const StencilState& rhs) const;
            bool operator!= (const StencilState& rhs) const;

            bool mStencilEnable;
            uint8_t mStencilReadMask;
            uint8_t mStencilWriteMask;
            StencilOp mFrontFace;
            StencilOp mBackFace;
        };

    public:
        DepthState mDepthState;
        StencilState mStencilState;
    };

    class DrawingRasterStateDesc : public DrawingResourceDesc
    {
    public:
        DrawingRasterStateDesc();
        DrawingRasterStateDesc(const DrawingRasterStateDesc& desc);
        DrawingRasterStateDesc(DrawingRasterStateDesc&& desc);
        virtual ~DrawingRasterStateDesc();

        DrawingRasterStateDesc& operator= (const DrawingRasterStateDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;

    public:
        EDrawingFillModeType mFillMode;
        EDrawingCullModeType mCullMode;

        bool mFrontCounterClockwise;

        
        int mDepthBias;

        float mDepthBiasClamp;
        float mSlopeScaledDepthBias;

        bool mDepthClipEnable;
        bool mScissorEnable;
        bool mMultisampleEnable;
        bool mAntialiasedLineEnable;
    };

    class DrawingSamplerStateDesc : public DrawingResourceDesc
    {
    public:
        DrawingSamplerStateDesc();
        DrawingSamplerStateDesc(const DrawingSamplerStateDesc& desc);
        DrawingSamplerStateDesc(DrawingSamplerStateDesc&& desc);
        virtual ~DrawingSamplerStateDesc();

        DrawingSamplerStateDesc& operator= (const DrawingSamplerStateDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;

    public:
        EDrawingSamplerModeType mSamplerMode;

        EDrawingFilterModeType mMinFilter;
        EDrawingFilterModeType mMagFilter;
        EDrawingFilterModeType mMipFilter;

        EDrawingAddressModeType mAddressU;
        EDrawingAddressModeType mAddressV;
        EDrawingAddressModeType mAddressW;

        EDrawingComparisonFuncType mComparisonFunc;

        float mBorderColor[4];
        float mMinLOD;
        float mMaxLOD;
        float mMipLODBias;

        uint32_t mMaxAnisotropy;
    };

    class DrawingTargetDesc : public DrawingResourceDesc
    {
    public:
        DrawingTargetDesc();
        DrawingTargetDesc(const DrawingTargetDesc& desc);
        DrawingTargetDesc(DrawingTargetDesc&& desc);
        virtual ~DrawingTargetDesc();

        DrawingTargetDesc& operator= (const DrawingTargetDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;

    public:
        void* mHwnd;
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mSlices;
        EDrawingFormatType mFormat;
        uint32_t mMultiSampleCount;
        uint32_t mMultiSampleQuality;
        uint32_t mFlags;
        uint32_t mRefreshRate;
        EDrawingSwapChainType mSwapChain;
    };

    class DrawingDepthBufferDesc : public DrawingTargetDesc
    {
    public:
        DrawingDepthBufferDesc();
        DrawingDepthBufferDesc(const DrawingDepthBufferDesc& desc);
        DrawingDepthBufferDesc(DrawingDepthBufferDesc&& desc);
        virtual ~DrawingDepthBufferDesc();

        DrawingDepthBufferDesc& operator= (const DrawingDepthBufferDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;
    };

    class DrawingTextureDesc : public DrawingResourceDesc
    {
    public:
        DrawingTextureDesc();
        DrawingTextureDesc(const DrawingTextureDesc& desc);
        DrawingTextureDesc(DrawingTextureDesc&& desc);
        virtual ~DrawingTextureDesc();

        DrawingTextureDesc& operator= (const DrawingTextureDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;

    public:
        EDrawingTextureType mType;
        EDrawingFormatType mFormat;
        EDrawingUsageType mUsage;
        EDrawingAccessType mAccess;
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mDepth;
        uint32_t mArraySize;
        uint32_t mMipLevels;
        uint32_t mBytesPerRow;
        uint32_t mBytesPerSlice;
        uint32_t mFlags;

        uint32_t mSampleCount;
        uint32_t mSampleQuality;
    };

    class DrawingPrimitiveDesc : public DrawingResourceDesc
    {
    public:
        DrawingPrimitiveDesc();
        DrawingPrimitiveDesc(const DrawingPrimitiveDesc& desc);
        DrawingPrimitiveDesc(DrawingPrimitiveDesc&& desc);
        virtual ~DrawingPrimitiveDesc();

        DrawingPrimitiveDesc& operator= (const DrawingPrimitiveDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;

    public:
        EDrawingPrimitiveType mPrimitive;
    };

    class DrawingVaringStatesDesc : public DrawingResourceDesc
    {
    public:
        DrawingVaringStatesDesc();
        DrawingVaringStatesDesc(const DrawingVaringStatesDesc& desc);
        DrawingVaringStatesDesc(DrawingVaringStatesDesc&& desc);
        virtual ~DrawingVaringStatesDesc();

        DrawingVaringStatesDesc& operator= (const DrawingVaringStatesDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;
    };

    class DrawingPipelineStateDesc : public DrawingResourceDesc
    {
    public:
        DrawingPipelineStateDesc();
        DrawingPipelineStateDesc(const DrawingPipelineStateDesc& desc);
        DrawingPipelineStateDesc(DrawingPipelineStateDesc&& desc);
        virtual ~DrawingPipelineStateDesc();

        enum EPipelineStateSubobjectType
        {
            ePipelineStateSubobjectType_RootSignature = 0,
            ePipelineStateSubobjectType_Vs,
            ePipelineStateSubobjectType_Ps,
            ePipelineStateSubobjectType_Ds,
            ePipelineStateSubobjectType_Hs,
            ePipelineStateSubobjectType_Gs,
            ePipelineStateSubobjectType_Cs,
            ePipelineStateSubobjectType_StreamOutput,
            ePipelineStateSubobjectType_BlendState,
            ePipelineStateSubobjectType_RasterState,
            ePipelineStateSubobjectType_DepthStencilState,
            ePipelineStateSubobjectType_InputLayout,
            ePipelineStateSubobjectType_PrimitiveTopology,
            ePipelineStateSubobjectType_RenderTarget,
            ePipelineStateSubobjectType_DepthStencil,
        };

        DrawingPipelineStateDesc& operator= (const DrawingPipelineStateDesc& rhs);

        EDrawingResourceType GetType() const override;
        DrawingResourceDesc* Clone() const override;

        void AttachSubobject(EPipelineStateSubobjectType type, std::shared_ptr<std::string> subobject);

        typedef std::unordered_map<EPipelineStateSubobjectType, std::shared_ptr<std::string>> SubobjectTable;

    private:
        void CloneFromSubobject(const SubobjectTable& from);

    public:
        SubobjectTable mSubobjectTable;
    };
}