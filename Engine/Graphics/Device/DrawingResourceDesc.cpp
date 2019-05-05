#include "DrawingResourceDesc.h"

using namespace Engine;

DrawingResourceDesc::DrawingResourceDesc(const DrawingResourceDesc& desc)
{
    CloneFromNames(desc.m_resourceDescNames);
}

DrawingResourceDesc::DrawingResourceDesc(DrawingResourceDesc&& desc)
{
    CloneFromNames(desc.m_resourceDescNames);
    m_resourceDescNames.clear();
}

DrawingResourceDesc::~DrawingResourceDesc()
{
    m_resourceDescNames.clear();
}

DrawingResourceDesc& DrawingResourceDesc::operator= (const DrawingResourceDesc& rhs)
{
    if (this == &rhs)
        return *this;

    CloneFromNames(rhs.m_resourceDescNames);
    return *this;
}

DrawingResourceDesc::ResourceDescNamesType DrawingResourceDesc::GetResourceDescNames() const
{
    return m_resourceDescNames;
}

void DrawingResourceDesc::AddResourceDescName(uint32_t index, std::shared_ptr<std::string> pName)
{
    auto it = m_resourceDescNames.find(index);
    if (it != m_resourceDescNames.cend())
        return;

    m_resourceDescNames.emplace(index, pName);
}

std::shared_ptr<std::string> DrawingResourceDesc::GetResourceDescName(uint32_t index) const
{
    auto it = m_resourceDescNames.find(index);
    if (it == m_resourceDescNames.cend())
        return nullptr;
    
    return it->second;
}

bool DrawingResourceDesc::IsExternalResource() const
{
    return m_externalRes;
}

void DrawingResourceDesc::SetIsExternalResource(bool flag)
{
    m_externalRes = flag;
}

void DrawingResourceDesc::CloneFromNames(const ResourceDescNamesType& from)
{
    m_resourceDescNames.clear();
    std::for_each(from.cbegin(), from.cend(), [this](const ResourceDescNamesType::value_type& aElem)
    {
        if (auto& lpName = aElem.second)
            m_resourceDescNames.emplace(aElem.first, lpName);
    });
}

DrawingProgramDesc::DrawingProgramDesc() : DrawingResourceDesc(),
    mProgramType(eProgram_Shader), mpName(nullptr), mpSourceName(nullptr)
{
}

DrawingProgramDesc::DrawingProgramDesc(const DrawingProgramDesc& desc) : DrawingResourceDesc(desc),
    mProgramType(desc.mProgramType), mpName(desc.mpName), mpSourceName(desc.mpSourceName)
{
}

DrawingProgramDesc::DrawingProgramDesc(DrawingProgramDesc&& desc) : DrawingResourceDesc(std::move(desc)),
    mProgramType(std::move(desc.mProgramType)), mpName(std::move(desc.mpName)), mpSourceName(std::move(desc.mpSourceName))
{
}

DrawingProgramDesc::~DrawingProgramDesc()
{
    mProgramType = eProgram_Shader;
    mpName = nullptr;
    mpSourceName = nullptr;
}

DrawingProgramDesc& DrawingProgramDesc::operator= (const DrawingProgramDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);
    mProgramType = rhs.mProgramType;
    mpName = rhs.mpName;
    mpSourceName = rhs.mpSourceName;
    return *this;
}

DrawingShaderDesc::DrawingShaderDesc() : DrawingProgramDesc(),
    mpEntryName(nullptr), mpIncludePath(nullptr)
{
}

DrawingShaderDesc::DrawingShaderDesc(const DrawingShaderDesc& desc) : DrawingProgramDesc(desc),
    mpEntryName(desc.mpEntryName), mpIncludePath(desc.mpIncludePath)
{
}

DrawingShaderDesc::DrawingShaderDesc(DrawingShaderDesc&& desc) : DrawingProgramDesc(std::move(desc)),
    mpEntryName(std::move(desc.mpEntryName)), mpIncludePath(std::move(desc.mpIncludePath))
{
}

DrawingShaderDesc::~DrawingShaderDesc()
{
    mpEntryName = nullptr;
    mpIncludePath = nullptr;
}

DrawingShaderDesc& DrawingShaderDesc::operator= (const DrawingShaderDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingProgramDesc::operator= (rhs);
    mpEntryName = rhs.mpEntryName;
    mpIncludePath = rhs.mpIncludePath;
    return *this;
}

DrawingVertexShaderDesc::DrawingVertexShaderDesc() : DrawingShaderDesc()
{
}

DrawingVertexShaderDesc::DrawingVertexShaderDesc(const DrawingVertexShaderDesc& desc) : DrawingShaderDesc(desc)
{
}

DrawingVertexShaderDesc::DrawingVertexShaderDesc(DrawingVertexShaderDesc&& desc) : DrawingShaderDesc(std::move(desc))
{
}

DrawingVertexShaderDesc::~DrawingVertexShaderDesc()
{
}

DrawingVertexShaderDesc& DrawingVertexShaderDesc::operator= (const DrawingVertexShaderDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingShaderDesc::operator= (rhs);
    return *this;
}

EDrawingResourceType DrawingVertexShaderDesc::GetType() const
{
    return eResource_Vertex_Shader;
}

DrawingResourceDesc* DrawingVertexShaderDesc::Clone() const
{
    return new DrawingVertexShaderDesc(*this);
}

DrawingPixelShaderDesc::DrawingPixelShaderDesc() : DrawingShaderDesc()
{
}

DrawingPixelShaderDesc::DrawingPixelShaderDesc(const DrawingPixelShaderDesc& desc) : DrawingShaderDesc(desc)
{
}

DrawingPixelShaderDesc::DrawingPixelShaderDesc(DrawingPixelShaderDesc&& desc) : DrawingShaderDesc(std::move(desc))
{
}

DrawingPixelShaderDesc::~DrawingPixelShaderDesc()
{
}

DrawingPixelShaderDesc& DrawingPixelShaderDesc::operator= (const DrawingPixelShaderDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingShaderDesc::operator= (rhs);
    return *this;
}

EDrawingResourceType DrawingPixelShaderDesc::GetType() const
{
    return eResource_Pixel_Shader;
}

DrawingResourceDesc* DrawingPixelShaderDesc::Clone() const
{
    return new DrawingPixelShaderDesc(*this);
}

DrawingEffectDesc::DrawingEffectDesc() : DrawingProgramDesc()
{
}

DrawingEffectDesc::DrawingEffectDesc(const DrawingEffectDesc& desc) : DrawingProgramDesc(desc)
{
}

DrawingEffectDesc::DrawingEffectDesc(DrawingEffectDesc&& desc) : DrawingProgramDesc(std::move(desc))
{
}

DrawingEffectDesc::~DrawingEffectDesc()
{
}

DrawingEffectDesc& DrawingEffectDesc::operator= (const DrawingEffectDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingProgramDesc::operator= (rhs);
    return *this;
}

EDrawingResourceType DrawingEffectDesc::GetType() const
{
    return eResource_Effect;
}

DrawingGeneralEffectDesc::DrawingGeneralEffectDesc() : DrawingEffectDesc(), mpTechName(nullptr)
{
}

DrawingGeneralEffectDesc::DrawingGeneralEffectDesc(const DrawingGeneralEffectDesc& desc) : DrawingEffectDesc(desc), mpTechName(desc.mpTechName)
{
}

DrawingGeneralEffectDesc::DrawingGeneralEffectDesc(DrawingGeneralEffectDesc&& desc) : DrawingEffectDesc(std::move(desc)), mpTechName(std::move(desc.mpTechName))
{
}

DrawingGeneralEffectDesc::~DrawingGeneralEffectDesc()
{
    mpTechName = nullptr;
}

DrawingGeneralEffectDesc& DrawingGeneralEffectDesc::operator= (const DrawingGeneralEffectDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingEffectDesc::operator= (rhs);
    mpTechName = rhs.mpTechName;
    return *this;
}

DrawingEffectDesc::EffectType DrawingGeneralEffectDesc::GetEffectType() const
{
    return EffectType_General;
}

DrawingResourceDesc* DrawingGeneralEffectDesc::Clone() const
{
    return new DrawingGeneralEffectDesc(*this);
}

DrawingComputeEffectDesc::DrawingComputeEffectDesc() : DrawingEffectDesc()
{
}

DrawingComputeEffectDesc::DrawingComputeEffectDesc(const DrawingComputeEffectDesc& desc) : DrawingEffectDesc(desc)
{
}

DrawingComputeEffectDesc::DrawingComputeEffectDesc(DrawingComputeEffectDesc&& desc) : DrawingEffectDesc(std::move(desc))
{
}

DrawingComputeEffectDesc::~DrawingComputeEffectDesc()
{
}

DrawingComputeEffectDesc& DrawingComputeEffectDesc::operator= (const DrawingComputeEffectDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingEffectDesc::operator= (rhs);
    return *this;
}

DrawingEffectDesc::EffectType DrawingComputeEffectDesc::GetEffectType() const
{
    return EffectType_Compute;
}

DrawingResourceDesc* DrawingComputeEffectDesc::Clone() const
{
    return new DrawingComputeEffectDesc(*this);
}

DrawingLinkedEffectDesc::DrawingLinkedEffectDesc() : DrawingEffectDesc()
{
}

DrawingLinkedEffectDesc::DrawingLinkedEffectDesc(const DrawingLinkedEffectDesc& desc) : DrawingEffectDesc(desc)
{
}

DrawingLinkedEffectDesc::DrawingLinkedEffectDesc(DrawingLinkedEffectDesc&& desc) : DrawingEffectDesc(std::move(desc))
{
}

DrawingLinkedEffectDesc::~DrawingLinkedEffectDesc()
{
}

DrawingLinkedEffectDesc& DrawingLinkedEffectDesc::operator= (const DrawingLinkedEffectDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingEffectDesc::operator= (rhs);
    return *this;
}

DrawingEffectDesc::EffectType DrawingLinkedEffectDesc::GetEffectType() const
{
    return EffectType_Linked;
}

DrawingResourceDesc* DrawingLinkedEffectDesc::Clone() const
{
    return new DrawingLinkedEffectDesc(*this);
}

DrawingVertexFormatDesc::VertexInputElement::VertexInputElement() : mpName(nullptr),
    mIndex(0), mFormat(eFormat_R32G32B32_FLOAT), mSlot(0), mOffset(0), mInstanceStepRate(0)
{
}

DrawingVertexFormatDesc::VertexInputElement::VertexInputElement(const VertexInputElement& elem) : mpName(elem.mpName),
    mIndex(elem.mIndex), mFormat(elem.mFormat), mSlot(elem.mSlot), mOffset(elem.mOffset), mInstanceStepRate(elem.mInstanceStepRate)
{
}

DrawingVertexFormatDesc::VertexInputElement::VertexInputElement(VertexInputElement&& elem) : mpName(std::move(elem.mpName)),
    mIndex(std::move(elem.mIndex)), mFormat(std::move(elem.mFormat)), mSlot(std::move(elem.mSlot)), mOffset(std::move(elem.mOffset)), mInstanceStepRate(std::move(elem.mInstanceStepRate))
{
}

DrawingVertexFormatDesc::VertexInputElement::~VertexInputElement()
{
    mpName = nullptr;
    mIndex = 0;
    mFormat = eFormat_R32G32B32_FLOAT;
    mSlot = 0;
    mOffset = 0;
    mInstanceStepRate = 0;
}

DrawingVertexFormatDesc::VertexInputElement& DrawingVertexFormatDesc::VertexInputElement::operator= (const VertexInputElement& rhs)
{
    if (this == &rhs)
        return *this;

    mpName = rhs.mpName;
    mIndex = rhs.mIndex;
    mFormat = rhs.mFormat;
    mSlot = rhs.mSlot;
    mOffset = rhs.mOffset;
    mInstanceStepRate = rhs.mInstanceStepRate;
    return *this;
}

DrawingVertexFormatDesc::DrawingVertexFormatDesc() : DrawingResourceDesc()
{
}

DrawingVertexFormatDesc::DrawingVertexFormatDesc(const DrawingVertexFormatDesc& desc) : DrawingResourceDesc(desc)
{
    m_inputElements = desc.m_inputElements;
}

DrawingVertexFormatDesc::DrawingVertexFormatDesc(DrawingVertexFormatDesc&& desc) : DrawingResourceDesc(std::move(desc))
{
    m_inputElements = std::move(desc.m_inputElements);
}

DrawingVertexFormatDesc::~DrawingVertexFormatDesc()
{
    m_inputElements.clear();
}

DrawingVertexFormatDesc& DrawingVertexFormatDesc::operator= (const DrawingVertexFormatDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);
    m_inputElements = rhs.m_inputElements;

    return *this;
}

EDrawingResourceType DrawingVertexFormatDesc::GetType() const
{
    return eResource_Vertex_Format;
}

DrawingResourceDesc* DrawingVertexFormatDesc::Clone() const
{
    return new DrawingVertexFormatDesc(*this);
}

DrawingBufferDesc::DrawingBufferDesc() : DrawingResourceDesc(),
    mSizeInBytes(0), mStrideInBytes(0), mUsage(eUsage_Default), mAccess(eAccess_No_Access), mFlags(0)
{
}

DrawingBufferDesc::DrawingBufferDesc(const DrawingBufferDesc& desc) : DrawingResourceDesc(desc),
    mSizeInBytes(desc.mSizeInBytes), mStrideInBytes(desc.mStrideInBytes), mUsage(desc.mUsage), mAccess(desc.mAccess), mFlags(desc.mFlags)
{
}

DrawingBufferDesc::DrawingBufferDesc(DrawingBufferDesc&& desc) : DrawingResourceDesc(std::move(desc)),
    mSizeInBytes(std::move(desc.mSizeInBytes)), mStrideInBytes(std::move(desc.mStrideInBytes)), mUsage(std::move(desc.mUsage)), mAccess(std::move(desc.mAccess)), mFlags(std::move(mFlags))
{
}

DrawingBufferDesc::~DrawingBufferDesc()
{
    mSizeInBytes = 0;
    mStrideInBytes = 0;
    mUsage = eUsage_Default;
    mAccess = eAccess_No_Access;
    mFlags = 0;
}

DrawingBufferDesc& DrawingBufferDesc::operator= (const DrawingBufferDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);
    mSizeInBytes = rhs.mSizeInBytes;
    mStrideInBytes = rhs.mStrideInBytes;
    mUsage = rhs.mUsage;
    mAccess = rhs.mAccess;
    mFlags = rhs.mFlags;

    return *this;
}

DrawingVertexBufferDesc::DrawingVertexBufferDesc() : DrawingBufferDesc()
{
}

DrawingVertexBufferDesc::DrawingVertexBufferDesc(const DrawingVertexBufferDesc& desc) : DrawingBufferDesc(desc)
{
}

DrawingVertexBufferDesc::DrawingVertexBufferDesc(DrawingVertexBufferDesc&& desc) : DrawingBufferDesc(std::move(desc))
{
}

DrawingVertexBufferDesc::~DrawingVertexBufferDesc()
{
}

DrawingVertexBufferDesc& DrawingVertexBufferDesc::operator= (const DrawingVertexBufferDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);

    return *this;
}

EDrawingResourceType DrawingVertexBufferDesc::GetType() const
{
    return eResource_Vertex_Buffer;
}

DrawingResourceDesc* DrawingVertexBufferDesc::Clone() const
{
    return new DrawingVertexBufferDesc(*this);
}

DrawingIndexBufferDesc::DrawingIndexBufferDesc() : DrawingBufferDesc()
{
}

DrawingIndexBufferDesc::DrawingIndexBufferDesc(const DrawingIndexBufferDesc& desc) : DrawingBufferDesc(desc)
{
}

DrawingIndexBufferDesc::DrawingIndexBufferDesc(DrawingIndexBufferDesc&& desc) : DrawingBufferDesc(std::move(desc))
{
}

DrawingIndexBufferDesc::~DrawingIndexBufferDesc()
{
}

DrawingIndexBufferDesc& DrawingIndexBufferDesc::operator= (const DrawingIndexBufferDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);

    return *this;
}

EDrawingResourceType DrawingIndexBufferDesc::GetType() const
{
    return eResource_Index_Buffer;
}

DrawingResourceDesc* DrawingIndexBufferDesc::Clone() const
{
    return new DrawingIndexBufferDesc(*this);
}

DrawingBlendStateDesc::BlendDef::BlendDef() : mBlendSrc(eBlend_SrcAlpha), mBlendDst(eBlend_InvSrcAlpha), mBlendOp(eBlendOp_Add)
{
}

DrawingBlendStateDesc::BlendDef::BlendDef(const BlendDef& blend) : mBlendSrc(blend.mBlendSrc), mBlendDst(blend.mBlendDst)
{
}

DrawingBlendStateDesc::BlendDef::BlendDef(BlendDef&& blend) : mBlendSrc(std::move(blend.mBlendSrc)), mBlendDst(std::move(blend.mBlendDst)), mBlendOp(std::move(blend.mBlendOp))
{
}

DrawingBlendStateDesc::BlendDef::~BlendDef()
{
    mBlendSrc = eBlend_SrcAlpha;
    mBlendDst = eBlend_InvSrcAlpha;
    mBlendOp = eBlendOp_Add;
}

DrawingBlendStateDesc::BlendDef& DrawingBlendStateDesc::BlendDef::operator= (const BlendDef& rhs)
{
    if (this == &rhs)
        return *this;

    mBlendSrc = rhs.mBlendSrc;
    mBlendDst = rhs.mBlendDst;
    mBlendOp = rhs.mBlendOp;

    return *this;
}

bool DrawingBlendStateDesc::BlendDef::operator== (const BlendDef& rhs) const
{
    if (this == &rhs)
        return true;

    return (mBlendSrc == rhs.mBlendSrc) && (mBlendDst == rhs.mBlendDst) && (mBlendOp == rhs.mBlendOp);
}

bool DrawingBlendStateDesc::BlendDef::operator!= (const BlendDef& rhs) const
{
    return !((*this) == rhs);
}

DrawingBlendStateDesc::BlendTarget::BlendTarget() : mBlendEnable(true), mRenderTargetWriteMask(WriteMast_All)
{
}

DrawingBlendStateDesc::BlendTarget::BlendTarget(const BlendTarget& target) : mBlendEnable(target.mBlendEnable), mRenderTargetWriteMask(target.mRenderTargetWriteMask)
{
}

DrawingBlendStateDesc::BlendTarget::BlendTarget(BlendTarget&& target) : mBlendEnable(std::move(target.mBlendEnable)), mRenderTargetWriteMask(std::move(target.mRenderTargetWriteMask))
{
}

DrawingBlendStateDesc::BlendTarget::~BlendTarget()
{
    mBlendEnable = true;
    mRenderTargetWriteMask = WriteMast_All;
}

DrawingBlendStateDesc::BlendTarget& DrawingBlendStateDesc::BlendTarget::operator= (const BlendTarget& rhs)
{
    if (this == &rhs)
        return *this;

    mBlendEnable = rhs.mBlendEnable;
    mAlphaBlend = rhs.mAlphaBlend;
    mColorBlend = rhs.mColorBlend;
    mRenderTargetWriteMask = rhs.mRenderTargetWriteMask;

    return *this;
}

bool DrawingBlendStateDesc::BlendTarget::operator== (const BlendTarget& rhs) const
{
    if (this == &rhs)
        return true;

    return (mBlendEnable == rhs.mBlendEnable) &&
           (mAlphaBlend == rhs.mAlphaBlend) &&
           (mColorBlend == rhs.mColorBlend) &&
           (mRenderTargetWriteMask == rhs.mRenderTargetWriteMask);
}

bool DrawingBlendStateDesc::BlendTarget::operator!= (const BlendTarget& rhs) const
{
    return !((*this) == rhs);
}

DrawingBlendStateDesc::DrawingBlendStateDesc() : DrawingResourceDesc(), mAlphaToCoverageEnable(true), mIndependentBlendEnable(false)
{
}

DrawingBlendStateDesc::DrawingBlendStateDesc(const DrawingBlendStateDesc& desc) : DrawingResourceDesc(desc), mAlphaToCoverageEnable(desc.mAlphaToCoverageEnable), mIndependentBlendEnable(desc.mIndependentBlendEnable)
{
    for (uint32_t i = 0; i < MAX_TARGETS; i++)
        mTargets[i] = desc.mTargets[i];
}

DrawingBlendStateDesc::DrawingBlendStateDesc(DrawingBlendStateDesc&& desc) : DrawingResourceDesc(std::move(desc)), mAlphaToCoverageEnable(std::move(desc.mAlphaToCoverageEnable)), mIndependentBlendEnable(std::move(desc.mIndependentBlendEnable))
{
    for (uint32_t i = 0; i < MAX_TARGETS; i++)
        mTargets[i] = std::move(desc.mTargets[i]);
}

DrawingBlendStateDesc::~DrawingBlendStateDesc()
{
    mAlphaToCoverageEnable = true;
    mIndependentBlendEnable = true;
}

DrawingBlendStateDesc& DrawingBlendStateDesc::operator= (const DrawingBlendStateDesc& rhs)
{
    if (this == &rhs)
        return *this;

    mAlphaToCoverageEnable = rhs.mAlphaToCoverageEnable;
    mIndependentBlendEnable = rhs.mIndependentBlendEnable;

    for (uint32_t i = 0; i < MAX_TARGETS; i++)
        mTargets[i] = rhs.mTargets[i];

    return *this;
}

EDrawingResourceType DrawingBlendStateDesc::GetType() const
{
    return eResource_Blend_State;
}

DrawingResourceDesc* DrawingBlendStateDesc::Clone() const
{
    return new DrawingBlendStateDesc(*this);
}

DrawingDepthStateDesc::DepthState::DepthState() : mDepthEnable(true), mDepthWriteEnable(true), mDepthFunc(eComparison_LessEqual)
{
}

DrawingDepthStateDesc::DepthState::DepthState(const DepthState& depth) : mDepthEnable(depth.mDepthEnable), mDepthWriteEnable(depth.mDepthWriteEnable), mDepthFunc(depth.mDepthFunc)
{
}

DrawingDepthStateDesc::DepthState::DepthState(DepthState&& depth) : mDepthEnable(std::move(depth.mDepthEnable)), mDepthWriteEnable(std::move(depth.mDepthWriteEnable)), mDepthFunc(std::move(depth.mDepthFunc))
{
}

DrawingDepthStateDesc::DepthState::~DepthState()
{
    mDepthEnable = true;
    mDepthWriteEnable = true;
    mDepthFunc = eComparison_LessEqual;
}

DrawingDepthStateDesc::DepthState& DrawingDepthStateDesc::DepthState::operator= (const DepthState& rhs)
{
    if (this == &rhs)
        return *this;

    mDepthEnable = rhs.mDepthEnable;
    mDepthWriteEnable = rhs.mDepthWriteEnable;
    mDepthFunc = rhs.mDepthFunc;

    return *this;
}

bool DrawingDepthStateDesc::DepthState::operator== (const DepthState& rhs) const
{
    if (this == &rhs)
        return true;

    return (mDepthEnable == rhs.mDepthEnable) &&
           (mDepthWriteEnable == rhs.mDepthWriteEnable) &&
           (mDepthFunc == rhs.mDepthFunc);
}

bool DrawingDepthStateDesc::DepthState::operator!= (const DepthState& rhs) const
{
    return !((*this) == rhs);
}

DrawingDepthStateDesc::StencilOp::StencilOp() : mStencilFailOp(eStencilOp_Keep), mStencilDepthFailOp(eStencilOp_Keep), mStencilPassOp(eStencilOp_Keep), mStencilFunc(eComparison_Always)
{
}

DrawingDepthStateDesc::StencilOp::StencilOp(const StencilOp& op) : mStencilFailOp(op.mStencilFailOp), mStencilDepthFailOp(op.mStencilDepthFailOp), mStencilPassOp(op.mStencilPassOp), mStencilFunc(op.mStencilFunc)
{
}

DrawingDepthStateDesc::StencilOp::StencilOp(StencilOp&& op) : mStencilFailOp(std::move(op.mStencilFailOp)), mStencilDepthFailOp(std::move(op.mStencilDepthFailOp)), mStencilPassOp(std::move(op.mStencilPassOp)), mStencilFunc(std::move(op.mStencilFunc))
{
}

DrawingDepthStateDesc::StencilOp::~StencilOp()
{
    mStencilFailOp = eStencilOp_Keep;
    mStencilDepthFailOp = eStencilOp_Keep;
    mStencilPassOp = eStencilOp_Keep;
    mStencilFunc = eComparison_Always;
}

DrawingDepthStateDesc::StencilOp& DrawingDepthStateDesc::StencilOp::operator= (const StencilOp& rhs)
{
    if (this == &rhs)
        return *this;

    mStencilFailOp = rhs.mStencilFailOp;
    mStencilDepthFailOp = rhs.mStencilDepthFailOp;
    mStencilPassOp = rhs.mStencilPassOp;
    mStencilFunc = rhs.mStencilFunc;

    return *this;
}

bool DrawingDepthStateDesc::StencilOp::operator== (const StencilOp& rhs) const
{
    if (this == &rhs)
        return true;

    return (mStencilFailOp == rhs.mStencilFailOp) &&
           (mStencilDepthFailOp == rhs.mStencilDepthFailOp) &&
           (mStencilPassOp == rhs.mStencilPassOp) &&
           (mStencilFunc == rhs.mStencilFunc);
}

bool DrawingDepthStateDesc::StencilOp::operator!= (const StencilOp& rhs) const
{
    return !((*this) == rhs);
}

DrawingDepthStateDesc::StencilState::StencilState() : mStencilEnable(false), mStencilReadMask(Mast_All), mStencilWriteMask(Mast_All)
{
}

DrawingDepthStateDesc::StencilState::StencilState(const StencilState& stencil) : mStencilEnable(stencil.mStencilEnable),
    mStencilReadMask(stencil.mStencilReadMask), mStencilWriteMask(stencil.mStencilWriteMask),
    mFrontFace(stencil.mFrontFace), mBackFace(stencil.mBackFace)
{
}

DrawingDepthStateDesc::StencilState::StencilState(StencilState&& stencil) : mStencilEnable(std::move(stencil.mStencilEnable)),
    mStencilReadMask(std::move(stencil.mStencilReadMask)), mStencilWriteMask(std::move(stencil.mStencilWriteMask)),
    mFrontFace(std::move(stencil.mFrontFace)), mBackFace(std::move(stencil.mBackFace))
{
}

DrawingDepthStateDesc::StencilState::~StencilState()
{
    mStencilEnable = false;
    mStencilReadMask = Mast_All;
    mStencilWriteMask = Mast_All;
}

DrawingDepthStateDesc::StencilState& DrawingDepthStateDesc::StencilState::operator= (const StencilState& rhs)
{
    if (this == &rhs)
        return *this;

    mStencilEnable = rhs.mStencilEnable;
    mStencilReadMask = rhs.mStencilReadMask;
    mStencilWriteMask = rhs.mStencilWriteMask;
    mFrontFace = rhs.mFrontFace;
    mBackFace = rhs.mBackFace;

    return *this;
}

bool DrawingDepthStateDesc::StencilState::operator== (const StencilState& rhs) const
{
    if (this == &rhs)
        return true;

    return (mStencilEnable == rhs.mStencilEnable) &&
           (mStencilReadMask == rhs.mStencilReadMask) &&
           (mStencilWriteMask == rhs.mStencilWriteMask) &&
           (mFrontFace == rhs.mFrontFace) &&
           (mBackFace == rhs.mBackFace);
}

bool DrawingDepthStateDesc::StencilState::operator!= (const StencilState& rhs) const
{
    return !((*this) == rhs);
}

DrawingDepthStateDesc::DrawingDepthStateDesc() : DrawingResourceDesc()
{
}

DrawingDepthStateDesc::DrawingDepthStateDesc(const DrawingDepthStateDesc& desc) : DrawingResourceDesc(desc),
    mDepthState(desc.mDepthState), mStencilState(desc.mStencilState)
{
}

DrawingDepthStateDesc::DrawingDepthStateDesc(DrawingDepthStateDesc&& desc) : DrawingResourceDesc(std::move(desc)),
    mDepthState(std::move(desc.mDepthState)), mStencilState(std::move(desc.mStencilState))
{
}

DrawingDepthStateDesc::~DrawingDepthStateDesc()
{
}

DrawingDepthStateDesc& DrawingDepthStateDesc::operator= (const DrawingDepthStateDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);

    mDepthState = rhs.mDepthState;
    mStencilState = rhs.mStencilState;

    return *this;
}

EDrawingResourceType DrawingDepthStateDesc::GetType() const
{
    return eResource_Depth_State;
}

DrawingResourceDesc* DrawingDepthStateDesc::Clone() const
{
    return new DrawingDepthStateDesc(*this);
}

DrawingRasterStateDesc::DrawingRasterStateDesc() : DrawingResourceDesc(),
    mFillMode(eFillMode_Solid), mCullMode(eCullMode_None), mFrontCounterClockwise(true), mDepthBias(0),
    mDepthBiasClamp(0.0f), mSlopeScaledDepthBias(0.0f),
    mDepthClipEnable(true), mScissorEnable(false), mMultisampleEnable(false), mAntialiasedLineEnable(false)
{
}

DrawingRasterStateDesc::DrawingRasterStateDesc(const DrawingRasterStateDesc& desc) : DrawingResourceDesc(desc),
    mFillMode(desc.mFillMode), mCullMode(desc.mCullMode), mFrontCounterClockwise(desc.mFrontCounterClockwise), mDepthBias(desc.mDepthBias),
    mDepthBiasClamp(desc.mDepthBiasClamp), mSlopeScaledDepthBias(desc.mSlopeScaledDepthBias),
    mDepthClipEnable(desc.mDepthClipEnable), mScissorEnable(desc.mScissorEnable), mMultisampleEnable(desc.mMultisampleEnable), mAntialiasedLineEnable(desc.mAntialiasedLineEnable)
{
}

DrawingRasterStateDesc::DrawingRasterStateDesc(DrawingRasterStateDesc&& desc) : DrawingResourceDesc(std::move(desc)),
    mFillMode(std::move(desc.mFillMode)), mCullMode(std::move(desc.mCullMode)), mFrontCounterClockwise(std::move(desc.mFrontCounterClockwise)), mDepthBias(std::move(desc.mDepthBias)),
    mDepthBiasClamp(std::move(desc.mDepthBiasClamp)), mSlopeScaledDepthBias(std::move(desc.mSlopeScaledDepthBias)),
    mDepthClipEnable(std::move(desc.mDepthClipEnable)), mScissorEnable(std::move(desc.mScissorEnable)), mMultisampleEnable(std::move(desc.mMultisampleEnable)), mAntialiasedLineEnable(std::move(desc.mAntialiasedLineEnable))
{
}

DrawingRasterStateDesc::~DrawingRasterStateDesc()
{
    mFillMode = eFillMode_Solid;
    mCullMode = eCullMode_None;
    mFrontCounterClockwise = true;
    mDepthBias = 0;
    mDepthBiasClamp = 0.0f;
    mSlopeScaledDepthBias = 0.0f;
    mDepthClipEnable = true;
    mScissorEnable = false;
    mMultisampleEnable = false;
    mAntialiasedLineEnable = false;
}

DrawingRasterStateDesc& DrawingRasterStateDesc::operator= (const DrawingRasterStateDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);

    mFillMode = rhs.mFillMode;
    mCullMode = rhs.mCullMode;
    mFrontCounterClockwise = rhs.mFrontCounterClockwise;
    mDepthBias = rhs.mDepthBias;
    mDepthBiasClamp = rhs.mDepthBiasClamp;
    mSlopeScaledDepthBias = rhs.mSlopeScaledDepthBias;
    mDepthClipEnable = rhs.mDepthClipEnable;
    mScissorEnable = rhs.mScissorEnable;
    mMultisampleEnable = rhs.mMultisampleEnable;
    mAntialiasedLineEnable = rhs.mAntialiasedLineEnable;

    return *this;
}

EDrawingResourceType DrawingRasterStateDesc::GetType() const
{
    return eResource_Raster_State;
}

DrawingResourceDesc* DrawingRasterStateDesc::Clone() const
{
    return new DrawingRasterStateDesc(*this);
}

DrawingSamplerStateDesc::DrawingSamplerStateDesc() : DrawingResourceDesc(),
    mSamplerMode(eSamplerMode_Normal), mMinFilter(eFilterMode_Point), mMagFilter(eFilterMode_Point), mMipFilter(eFilterMode_Point),
    mAddressU(eAddressMode_Clamp), mAddressV(eAddressMode_Clamp), mAddressW(eAddressMode_Clamp),
    mComparisonFunc(eComparison_Always), mBorderColor{1.0f}, mMinLOD(0.0f), mMaxLOD(0.0f), mMipLODBias(0.0f), mMaxAnisotropy(1)
{
}

DrawingSamplerStateDesc::DrawingSamplerStateDesc(const DrawingSamplerStateDesc& desc) : DrawingResourceDesc(desc),
    mSamplerMode(desc.mSamplerMode), mMinFilter(desc.mMinFilter), mMagFilter(desc.mMagFilter), mMipFilter(desc.mMipFilter),
    mAddressU(desc.mAddressU), mAddressV(desc.mAddressV), mAddressW(desc.mAddressW),
    mComparisonFunc(desc.mComparisonFunc), mMinLOD(desc.mMinLOD), mMaxLOD(desc.mMaxLOD), mMipLODBias(desc.mMipLODBias), mMaxAnisotropy(desc.mMaxAnisotropy)
{
    for (int i = 0; i < 4; i++)
        mBorderColor[i] = desc.mBorderColor[i];
}

DrawingSamplerStateDesc::DrawingSamplerStateDesc(DrawingSamplerStateDesc&& desc) : DrawingResourceDesc(std::move(desc)),
    mSamplerMode(std::move(desc.mSamplerMode)), mMinFilter(std::move(desc.mMinFilter)), mMagFilter(std::move(desc.mMagFilter)), mMipFilter(std::move(desc.mMipFilter)),
    mAddressU(std::move(desc.mAddressU)), mAddressV(std::move(desc.mAddressV)), mAddressW(std::move(desc.mAddressW)),
    mComparisonFunc(std::move(desc.mComparisonFunc)), mMinLOD(std::move(desc.mMinLOD)), mMaxLOD(std::move(desc.mMaxLOD)), mMipLODBias(std::move(desc.mMipLODBias)), mMaxAnisotropy(std::move(desc.mMaxAnisotropy))
{
    for (int i = 0; i < 4; i++)
        mBorderColor[i] = std::move(desc.mBorderColor[i]);
}

DrawingSamplerStateDesc::~DrawingSamplerStateDesc()
{
    mSamplerMode = eSamplerMode_Normal;
    mMinFilter = eFilterMode_Point;
    mMagFilter = eFilterMode_Point;
    mMipFilter = eFilterMode_Point;
    mAddressU = eAddressMode_Clamp;
    mAddressV = eAddressMode_Clamp;
    mAddressW = eAddressMode_Clamp;
    mComparisonFunc = eComparison_Always;
    mMinLOD = 0.0f;
    mMaxLOD = 0.0f;
    mMipLODBias = 0.0f;
    mMaxAnisotropy = 1;
    for (int i = 0; i < 4; i++)
        mBorderColor[i] = 1.0f;
}

DrawingSamplerStateDesc& DrawingSamplerStateDesc::operator= (const DrawingSamplerStateDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);

    mSamplerMode = rhs.mSamplerMode;
    mMinFilter = rhs.mMinFilter;
    mMagFilter = rhs.mMagFilter;
    mMipFilter = rhs.mMipFilter;
    mAddressU = rhs.mAddressU;
    mAddressV = rhs.mAddressV;
    mAddressW = rhs.mAddressW;
    mComparisonFunc = rhs.mComparisonFunc;
    mMinLOD = rhs.mMinLOD;
    mMaxLOD = rhs.mMaxLOD;
    mMipLODBias = rhs.mMipLODBias;
    mMaxAnisotropy = rhs.mMaxAnisotropy;
    for (int i = 0; i < 4; i++)
        mBorderColor[i] = rhs.mBorderColor[i];

    return *this;
}

EDrawingResourceType DrawingSamplerStateDesc::GetType() const
{
    return eResource_Sampler_State;
}

DrawingResourceDesc* DrawingSamplerStateDesc::Clone() const
{
    return new DrawingSamplerStateDesc(*this);
}

DrawingTargetDesc::DrawingTargetDesc() : DrawingResourceDesc(),
    mHwnd(nullptr), mWidth(0), mHeight(0), mSlices(1), mFormat(eFormat_R8G8B8A8_UNORM),
    mMultiSampleCount(1), mMultiSampleQuality(0), mFlags(0), mRefreshRate(60), mSwapChain(eSwapChain_Discard)
{
}

DrawingTargetDesc::DrawingTargetDesc(const DrawingTargetDesc& desc) : DrawingResourceDesc(desc),
    mHwnd(desc.mHwnd), mWidth(desc.mWidth), mHeight(desc.mHeight), mSlices(desc.mSlices), mFormat(desc.mFormat),
    mMultiSampleCount(desc.mMultiSampleCount), mMultiSampleQuality(desc.mMultiSampleQuality), mFlags(desc.mFlags), mRefreshRate(desc.mRefreshRate), mSwapChain(desc.mSwapChain)
{
}

DrawingTargetDesc::DrawingTargetDesc(DrawingTargetDesc&& desc) : DrawingResourceDesc(std::move(desc)),
    mHwnd(std::move(desc.mHwnd)), mWidth(std::move(desc.mWidth)), mHeight(std::move(desc.mHeight)), mSlices(std::move(desc.mSlices)), mFormat(std::move(desc.mFormat)),
    mMultiSampleCount(std::move(desc.mMultiSampleCount)), mMultiSampleQuality(std::move(desc.mMultiSampleQuality)), mFlags(std::move(desc.mFlags)), mRefreshRate(std::move(desc.mRefreshRate)), mSwapChain(std::move(desc.mSwapChain))
{
}

DrawingTargetDesc::~DrawingTargetDesc()
{
    mHwnd = nullptr;
    mWidth = 0;
    mHeight = 0;
    mSlices = 1;
    mFormat = eFormat_R8G8B8A8_UNORM;
    mMultiSampleCount = 1;
    mMultiSampleQuality = 0;
    mFlags = 0;
    mRefreshRate = 60;
    mSwapChain = eSwapChain_Discard;
}

DrawingTargetDesc& DrawingTargetDesc::operator= (const DrawingTargetDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);

    mHwnd = rhs.mHwnd;
    mWidth = rhs.mWidth;
    mHeight = rhs.mHeight;
    mSlices = rhs.mSlices;
    mFormat = rhs.mFormat;
    mMultiSampleCount = rhs.mMultiSampleCount;
    mMultiSampleQuality = rhs.mMultiSampleQuality;
    mFlags = rhs.mFlags;
    mRefreshRate = rhs.mRefreshRate;
    mSwapChain = rhs.mSwapChain;

    return *this;
}

EDrawingResourceType DrawingTargetDesc::GetType() const
{
    return eResource_Target;
}

DrawingResourceDesc* DrawingTargetDesc::Clone() const
{
    return new DrawingTargetDesc(*this);
}

DrawingDepthBufferDesc::DrawingDepthBufferDesc() : DrawingTargetDesc()
{
    mFormat = eFormat_D24S8;
}

DrawingDepthBufferDesc::DrawingDepthBufferDesc(const DrawingDepthBufferDesc& desc) : DrawingTargetDesc(desc)
{
}

DrawingDepthBufferDesc::DrawingDepthBufferDesc(DrawingDepthBufferDesc&& desc) : DrawingTargetDesc(std::move(desc))
{
}

DrawingDepthBufferDesc::~DrawingDepthBufferDesc()
{
    mFormat = eFormat_D24S8;
}

DrawingDepthBufferDesc& DrawingDepthBufferDesc::operator= (const DrawingDepthBufferDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);

    return *this;
}

EDrawingResourceType DrawingDepthBufferDesc::GetType() const
{
    return eResource_DepthBuffer;
}

DrawingResourceDesc* DrawingDepthBufferDesc::Clone() const
{
    return new DrawingDepthBufferDesc(*this);
}

DrawingTextureDesc::DrawingTextureDesc() : DrawingResourceDesc(),
    mType(eTexture_2D), mFormat(eFormat_R8G8B8A8_UNORM), mUsage(eUsage_Default), mAccess(eAccess_No_Access),
    mWidth(0), mHeight(0), mDepth(0), mArraySize(1), mMipLevels(1), mBytesPerRow(0), mBytesPerSlice(0), mFlags(0),
    mSampleCount(1), mSampleQuality(0)
{
}

DrawingTextureDesc::DrawingTextureDesc(const DrawingTextureDesc& desc) : DrawingResourceDesc(desc),
    mType(desc.mType), mFormat(desc.mFormat), mUsage(desc.mUsage), mAccess(desc.mAccess),
    mWidth(desc.mWidth), mHeight(desc.mHeight), mDepth(desc.mDepth), mArraySize(desc.mArraySize), mMipLevels(desc.mMipLevels), mBytesPerRow(desc.mBytesPerRow), mBytesPerSlice(desc.mBytesPerSlice), mFlags(desc.mFlags),
    mSampleCount(desc.mSampleCount), mSampleQuality(desc.mSampleQuality) 
{
}

DrawingTextureDesc::DrawingTextureDesc(DrawingTextureDesc&& desc) : DrawingResourceDesc(std::move(desc)),
    mType(std::move(desc.mType)), mFormat(std::move(desc.mFormat)), mUsage(std::move(desc.mUsage)), mAccess(std::move(desc.mAccess)),
    mWidth(std::move(desc.mWidth)), mHeight(std::move(desc.mHeight)), mDepth(std::move(desc.mDepth)), mArraySize(std::move(desc.mArraySize)), mMipLevels(std::move(desc.mMipLevels)), mBytesPerRow(std::move(desc.mBytesPerRow)), mBytesPerSlice(std::move(desc.mBytesPerSlice)), mFlags(std::move(desc.mFlags)),
    mSampleCount(std::move(desc.mSampleCount)), mSampleQuality(std::move(desc.mSampleQuality))
{
}

DrawingTextureDesc::~DrawingTextureDesc()
{
    mType = eTexture_2D;
    mFormat = eFormat_R8G8B8A8_UNORM;
    mUsage = eUsage_Default;
    mAccess = eAccess_No_Access;
    mWidth = 0;
    mHeight = 0;
    mDepth = 0;
    mArraySize = 1;
    mMipLevels = 1;
    mBytesPerRow = 0;
    mBytesPerSlice = 0;
    mFlags = 0;
    mSampleCount = 1;
    mSampleQuality = 0;
}

DrawingTextureDesc& DrawingTextureDesc::operator= (const DrawingTextureDesc& rhs)
{
    if (this == &rhs)
        return *this;

    DrawingResourceDesc::operator= (rhs);
    mType = rhs.mType;
    mFormat = rhs.mFormat;
    mUsage = rhs.mUsage;
    mAccess = rhs.mAccess;
    mWidth = rhs.mWidth;
    mHeight = rhs.mHeight;
    mDepth = rhs.mDepth;
    mArraySize = rhs.mArraySize;
    mMipLevels = rhs.mMipLevels;
    mBytesPerRow = rhs.mBytesPerRow;
    mBytesPerSlice = rhs.mBytesPerSlice;
    mFlags = rhs.mFlags;
    mSampleCount = rhs.mSampleCount;
    mSampleQuality = rhs.mSampleQuality;

    return *this;
}

EDrawingResourceType DrawingTextureDesc::GetType() const
{
    return eResource_Texture;
}

DrawingResourceDesc* DrawingTextureDesc::Clone() const
{
    return new DrawingTextureDesc(*this);
}

DrawingPrimitiveDesc::DrawingPrimitiveDesc() : DrawingResourceDesc(), mPrimitive(ePrimitive_TriangleList)
{
}

DrawingPrimitiveDesc::DrawingPrimitiveDesc(const DrawingPrimitiveDesc& desc) : DrawingResourceDesc(desc), mPrimitive(desc.mPrimitive)
{
}

DrawingPrimitiveDesc::DrawingPrimitiveDesc(DrawingPrimitiveDesc&& desc) : DrawingResourceDesc(std::move(desc)), mPrimitive(std::move(desc.mPrimitive))
{
}

DrawingPrimitiveDesc::~DrawingPrimitiveDesc()
{
    mPrimitive = ePrimitive_TriangleList;
}

DrawingPrimitiveDesc& DrawingPrimitiveDesc::operator= (const DrawingPrimitiveDesc& rhs)
{
    if (this == &rhs)
        return *this;

    mPrimitive = rhs.mPrimitive;

    return *this;
}

EDrawingResourceType DrawingPrimitiveDesc::GetType() const
{
    return eResource_Primitive;
}

DrawingResourceDesc* DrawingPrimitiveDesc::Clone() const
{
    return new DrawingPrimitiveDesc(*this);
}

DrawingVaringStatesDesc::DrawingVaringStatesDesc() : DrawingResourceDesc()
{
}

DrawingVaringStatesDesc::DrawingVaringStatesDesc(const DrawingVaringStatesDesc& desc) : DrawingResourceDesc(desc)
{
}

DrawingVaringStatesDesc::DrawingVaringStatesDesc(DrawingVaringStatesDesc&& desc) : DrawingResourceDesc(std::move(desc))
{
}

DrawingVaringStatesDesc::~DrawingVaringStatesDesc()
{
}

DrawingVaringStatesDesc& DrawingVaringStatesDesc::operator= (const DrawingVaringStatesDesc& rhs)
{
    if (this == &rhs)
        return *this;

    return *this;
}

EDrawingResourceType DrawingVaringStatesDesc::GetType() const
{
    return eResource_Varing_States;
}

DrawingResourceDesc* DrawingVaringStatesDesc::Clone() const
{
    return new DrawingVaringStatesDesc(*this);
}

DrawingPipelineStateDesc::DrawingPipelineStateDesc() : DrawingResourceDesc()
{
    mSubobjectTable.clear();
}

DrawingPipelineStateDesc::DrawingPipelineStateDesc(const DrawingPipelineStateDesc& desc) : DrawingResourceDesc(desc)
{
    CloneFromSubobject(desc.mSubobjectTable);
}

DrawingPipelineStateDesc::DrawingPipelineStateDesc(DrawingPipelineStateDesc&& desc) : DrawingResourceDesc(std::move(desc))
{
    CloneFromSubobject(desc.mSubobjectTable);
    mSubobjectTable.clear();
}

DrawingPipelineStateDesc::~DrawingPipelineStateDesc()
{
    mSubobjectTable.clear();
}

DrawingPipelineStateDesc& DrawingPipelineStateDesc::operator= (const DrawingPipelineStateDesc& rhs)
{
    if (this == &rhs)
        return *this;

    CloneFromSubobject(rhs.mSubobjectTable);

    return *this;
}

EDrawingResourceType DrawingPipelineStateDesc::GetType() const
{
    return eResource_Pipeline_State;
}

DrawingResourceDesc* DrawingPipelineStateDesc::Clone() const
{
    return new DrawingPipelineStateDesc(*this);
}

void DrawingPipelineStateDesc::AttachSubobject(EPipelineStateSubobjectType type, std::shared_ptr<std::string> subobject)
{
    if (subobject == nullptr)
        return;

    mSubobjectTable[type] = subobject;
    AddResourceDescName(type, subobject);
}

void DrawingPipelineStateDesc::CloneFromSubobject(const SubobjectTable& from)
{
    mSubobjectTable.clear();
    std::for_each(from.cbegin(), from.cend(), [this](const SubobjectTable::value_type& aElem)
    {
        if (auto& lpName = aElem.second)
            mSubobjectTable.emplace(aElem.first, lpName);
    });
}