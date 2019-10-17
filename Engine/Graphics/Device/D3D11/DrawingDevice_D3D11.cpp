#include <fstream>
#include <memory>
#include <sstream>
#include <d3d11shader.h>
#include <d3dcompiler.h>

#include "Macros.h"
#include "DrawingUtil_D3D11.h"
#include "DrawingRawResource_D3D11.h"
#include "DrawingDevice_D3D11.h"
#include "DrawingParameter.h"

using namespace Engine;

DrawingDevice_D3D11::DrawingDevice_D3D11(const std::shared_ptr<ID3D11Device> device) : m_pDevice(device)
{
}

DrawingDevice_D3D11::~DrawingDevice_D3D11()
{
    Shutdown();
}

void DrawingDevice_D3D11::Initialize()
{
    assert(m_pDevice != nullptr);

    ID3D11DeviceContext* pDeviceContextRaw;
    IDXGIDevice* pDXGIDeviceRaw;
    IDXGIAdapter* pDXGIAdapterRaw;
    IDXGIFactory* pDXGIFactoryRaw;

    m_pDevice->GetImmediateContext(&pDeviceContextRaw);
    m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDeviceRaw);
    pDXGIDeviceRaw->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapterRaw);
    pDXGIAdapterRaw->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactoryRaw);

    m_pDeviceContext = std::shared_ptr<ID3D11DeviceContext>(pDeviceContextRaw, D3D11Releaser<ID3D11DeviceContext>);
    m_pDXGIDevice = std::shared_ptr<IDXGIDevice>(pDXGIDeviceRaw, D3D11Releaser<IDXGIDevice>);
    m_pDXGIAdapter = std::shared_ptr<IDXGIAdapter>(pDXGIAdapterRaw, D3D11Releaser<IDXGIAdapter>);
    m_pDXGIFactory = std::shared_ptr<IDXGIFactory>(pDXGIFactoryRaw, D3D11Releaser<IDXGIFactory>);
}

void DrawingDevice_D3D11::Shutdown()
{
}

std::string CompositeFakeEffectString(const std::vector<DrawingVertexFormatDesc::VertexInputElement>& elems)
{
    std::ostringstream fieldStream;
    uint32_t varSuffix = 0;

    for (const auto& elem : elems)
    {
        fieldStream << D3D11HLSLFormatToString(elem.mFormat) << " variable" << varSuffix++ << " : " << elem.mpName->c_str();
        if (elem.mIndex != 0)
            fieldStream << elem.mIndex;
        fieldStream << ";\n";
    }

    return "struct VertexIn\n{\n" + fieldStream.str() + "};\n" +
            "struct VertexOut\n{\nfloat4 Color : COLOR;\n};\nVertexOut VS(VertexIn vin)\n{\nVertexOut vout;\nvout.Color = float4(0, 0,  0, 0);\nreturn vout;\n}\n" +
            "float4 PS(VertexOut pin) : SV_Target\n{\nreturn pin.Color;\n}\n" +
            "technique11 D3D11_InputLayout_Creation_Fake_Tech\n{\npass P0\n{\nSetVertexShader(CompileShader(vs_5_0, VS()));\nSetPixelShader(CompileShader(ps_5_0, PS()));\n}\n}";
        
}

bool DrawingDevice_D3D11::CreateVertexFormat(const DrawingVertexFormatDesc& desc, std::shared_ptr<DrawingVertexFormat>& pRes)
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc;

    for (const auto& elem : desc.m_inputElements)
    {
        D3D11_INPUT_ELEMENT_DESC input;
        input.SemanticName = elem.mpName->c_str();
        input.SemanticIndex = elem.mIndex;
        input.Format = D3D11Enum(elem.mFormat);
        input.InputSlot = elem.mSlot;
        input.AlignedByteOffset = elem.mOffset;
        input.InputSlotClass = elem.mInstanceStepRate == 0 ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
        input.InstanceDataStepRate = elem.mInstanceStepRate;

        inputElementDesc.emplace_back(input);
    }

    auto pVertexFormat = std::make_shared<DrawingVertexFormat>(shared_from_this());

    DrawingGeneralEffectDesc effectDesc;
    effectDesc.mProgramType = eProgram_String;
    effectDesc.mpName = strPtr("D3D11_InputLayout_Creation_Fake_Effect");
    effectDesc.mpTechName = strPtr("D3D11_InputLayout_Creation_Fake_Tech");

    std::shared_ptr<DrawingEffect> pEffect;
    if (!CreateEffectFromString(CompositeFakeEffectString(desc.m_inputElements), effectDesc, pEffect))
        return false;

    auto pD3D11Effect = std::dynamic_pointer_cast<DrawingRawEffect_D3D11>(pEffect->GetResource());
    auto pInputSignature = pD3D11Effect->GetInputSignature();
    auto byteCodeLength = pD3D11Effect->GetByteCodeLength();

    std::shared_ptr<DrawingRawVertexFormat> pVertexFormatRaw = std::make_shared<DrawingRawVertexFormat_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), inputElementDesc, pInputSignature, (uint32_t)byteCodeLength);
    pVertexFormat->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pVertexFormat->SetResource(pVertexFormatRaw);

    pRes = pVertexFormat;
    return true;
}

bool DrawingDevice_D3D11::CreateVertexBuffer(const DrawingVertexBufferDesc& desc, std::shared_ptr<DrawingVertexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes, const void* pData, uint32_t size)
{
    if ((pData != nullptr) && (size > desc.mSizeInBytes))
        return false;

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.ByteWidth = desc.mSizeInBytes;
    bufferDesc.Usage = D3D11Enum(desc.mUsage);
    bufferDesc.CPUAccessFlags = D3D11Enum(desc.mAccess);
    bufferDesc.MiscFlags = D3D11ResourceMiscFlag(desc.mFlags);
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subResData;
    ZeroMemory(&subResData, sizeof(D3D11_SUBRESOURCE_DATA));
    subResData.pSysMem = pData;

    auto pVertexBuffer = std::make_shared<DrawingVertexBuffer>(shared_from_this());

    std::shared_ptr<DrawingRawVertexBuffer> pVertexBufferRaw = std::make_shared<DrawingRawVertexBuffer_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), bufferDesc, subResData, desc.mStrideInBytes, 0);
    pVertexBuffer->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pVertexBuffer->SetResource(pVertexBufferRaw);

    pRes = pVertexBuffer;
    return true;
}

bool DrawingDevice_D3D11::CreateIndexBuffer(const DrawingIndexBufferDesc& desc, std::shared_ptr<DrawingIndexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes, const void* pData, uint32_t size)
{
    if ((pData != nullptr) && (size > desc.mSizeInBytes))
        return false;

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.ByteWidth = desc.mSizeInBytes;
    bufferDesc.Usage = D3D11Enum(desc.mUsage);
    bufferDesc.CPUAccessFlags = D3D11Enum(desc.mAccess);
    bufferDesc.MiscFlags = D3D11ResourceMiscFlag(desc.mFlags);
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subResData;
    ZeroMemory(&subResData, sizeof(D3D11_SUBRESOURCE_DATA));
    subResData.pSysMem = pData;

    auto pIndexBuffer = std::make_shared<DrawingIndexBuffer>(shared_from_this());

    std::shared_ptr<DrawingRawIndexBuffer> pIndexBufferRaw = std::make_shared<DrawingRawIndexBuffer_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), bufferDesc, subResData, desc.mStrideInBytes, 0);
    pIndexBuffer->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pIndexBuffer->SetResource(pIndexBufferRaw);

    pRes = pIndexBuffer;
    return true;
}

bool DrawingDevice_D3D11::CreateTexture(const DrawingTextureDesc& desc, std::shared_ptr<DrawingTexture>& pRes, std::shared_ptr<DrawingResource> pRefRes, const void* pData[], uint32_t size[], uint32_t slices)
{
    auto pTexture = std::make_shared<DrawingTexture>(shared_from_this());
    std::shared_ptr<DrawingRawTexture> pRawTexture = nullptr;

    auto pTarget = std::dynamic_pointer_cast<DrawingTarget>(pRefRes);
    auto pDepthBuffer = std::dynamic_pointer_cast<DrawingDepthBuffer>(pRefRes);
    if (pDepthBuffer != nullptr)
    {
        auto pRawDepthBuffer = std::static_pointer_cast<DrawingRawDepthTarget_D3D11>(pDepthBuffer->GetResource());
        assert(pRawDepthBuffer != nullptr);

        pRawTexture = std::make_shared<DrawingRawTexture2D_D3D11>(*pRawDepthBuffer);
    }
    else if (pTarget != nullptr)
    {
        auto pRawTarget = std::static_pointer_cast<DrawingRawRenderTarget_D3D11>(pTarget->GetResource());
        assert(pRawTarget != nullptr);

        pRawTexture = std::make_shared<DrawingRawTexture2D_D3D11>(*pRawTarget);
    }
    else
    {
        std::vector<D3D11_SUBRESOURCE_DATA> subResData(slices);
        switch (desc.mType)
        {
            case eTexture_1D:
            case eTexture_1DArray:
            {
                break;
            }
            case eTexture_2D:
            case eTexture_2DArray:
            case eTexture_Cube:
            {
                D3D11_TEXTURE2D_DESC texture2DDesc;
                texture2DDesc.Width = desc.mWidth;
                texture2DDesc.Height = desc.mHeight;
                texture2DDesc.MipLevels = desc.mMipLevels;
                texture2DDesc.ArraySize = desc.mArraySize;
                texture2DDesc.Format = D3D11Enum(desc.mFormat);
                texture2DDesc.SampleDesc.Count = desc.mSampleCount;
                texture2DDesc.SampleDesc.Quality = desc.mSampleQuality;
                texture2DDesc.Usage = D3D11Enum(desc.mUsage);
                texture2DDesc.BindFlags = (texture2DDesc.Usage == D3D11_USAGE_STAGING) ? 0 : D3D11_BIND_SHADER_RESOURCE;
                texture2DDesc.CPUAccessFlags = D3D11Enum(desc.mAccess);
                texture2DDesc.MiscFlags = (texture2DDesc.Usage == D3D11_USAGE_STAGING) ? 0 : D3D11ResourceMiscFlag(desc.mFlags);
    
                if ((desc.mUsage != eUsage_Staging) && (desc.mFlags & eResource_Gen_Mips))
                    texture2DDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
    
                if (desc.mType == eTexture_Cube)
                {
                    texture2DDesc.ArraySize = 6;
                    texture2DDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
                }
    
                if (!subResData.empty())
                {
                    auto mipLevels = slices / desc.mArraySize;
                    for (uint32_t index = 0; index < desc.mArraySize; ++index)
                    {
                        auto bytesPerRow = desc.mBytesPerRow;
                        for (uint32_t level = 0; level < mipLevels; ++level)
                        {
                            auto LOD = index * level + level;
                            ZeroMemory(&subResData[LOD], sizeof(D3D11_SUBRESOURCE_DATA));
                            subResData[LOD].pSysMem = *(pData++);
                            subResData[LOD].SysMemPitch = bytesPerRow;
                            bytesPerRow = bytesPerRow > 1U ? bytesPerRow >> 1 : 1U;
                        }
                    }
                }
                pRawTexture = std::make_shared<DrawingRawTexture2D_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), texture2DDesc, subResData);
            }
        }
    }

    pTexture->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pTexture->SetResource(pRawTexture);

    pRes = pTexture;

    return true;
}

bool DrawingDevice_D3D11::CreateTextureFromFile(const std::string uri, std::shared_ptr<DrawingTexture>& pRes)
{
    auto pTexture = std::make_shared<DrawingTexture>(shared_from_this());

    std::shared_ptr<DrawingRawTexture> pRawTexture = std::make_shared<DrawingRawTexture2D_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), uri);
    pTexture->SetResource(pRawTexture);

    pRes = pTexture;

    return true;
}

bool DrawingDevice_D3D11::CreateTarget(const DrawingTargetDesc& desc, std::shared_ptr<DrawingTarget>& pRes)
{
    std::shared_ptr<DrawingRawTarget> pTargetRaw = nullptr;
    if (desc.mHwnd == nullptr)
    {
        D3D11_TEXTURE2D_DESC renderTargetDesc;
        renderTargetDesc.Width = desc.mWidth;
        renderTargetDesc.Height = desc.mHeight;
        renderTargetDesc.MipLevels = 1;
        renderTargetDesc.ArraySize = desc.mSlices;
        renderTargetDesc.Format = D3D11Enum(desc.mFormat);
        renderTargetDesc.SampleDesc.Count = desc.mMultiSampleCount;
        renderTargetDesc.SampleDesc.Quality = desc.mMultiSampleQuality;
        renderTargetDesc.Usage = D3D11_USAGE_DEFAULT;
        renderTargetDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        renderTargetDesc.CPUAccessFlags = 0;
        renderTargetDesc.MiscFlags = D3D11ResourceMiscFlag(desc.mFlags);

        pTargetRaw = std::make_shared<DrawingRawRenderTarget_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), renderTargetDesc);
    }
    else
    {
        DXGI_SWAP_CHAIN_DESC SwapChainDesc;
        SwapChainDesc.BufferDesc.Width = desc.mWidth;
        SwapChainDesc.BufferDesc.Height = desc.mHeight;
        SwapChainDesc.BufferDesc.RefreshRate.Numerator = desc.mRefreshRate;
        SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        SwapChainDesc.BufferDesc.Format = D3D11Enum(desc.mFormat);
        SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        SwapChainDesc.SampleDesc.Count = desc.mMultiSampleCount;
        SwapChainDesc.SampleDesc.Quality = desc.mMultiSampleQuality;
        SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        SwapChainDesc.BufferCount = BUFFER_COUNT;
        SwapChainDesc.OutputWindow = (HWND)desc.mHwnd;
        SwapChainDesc.Windowed = true;
        SwapChainDesc.SwapEffect = D3D11Enum(desc.mSwapChain);
        SwapChainDesc.Flags = 0;

        pTargetRaw = std::make_shared<DrawingRawSwapChain_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), SwapChainDesc);
    }

    auto pTarget = std::make_shared<DrawingTarget>(shared_from_this());
    pTarget->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pTarget->SetResource(pTargetRaw);

    pRes = pTarget;
    return true;
}

bool DrawingDevice_D3D11::CreateDepthBuffer(const DrawingDepthBufferDesc& desc, std::shared_ptr<DrawingDepthBuffer>& pRes)
{
    std::shared_ptr<DrawingRawTarget> pDepthTargetRaw = nullptr;

    D3D11_TEXTURE2D_DESC depthTargetDesc;
    depthTargetDesc.Width = desc.mWidth;
    depthTargetDesc.Height = desc.mHeight;
    depthTargetDesc.MipLevels = 1;
    depthTargetDesc.ArraySize = desc.mSlices;
    depthTargetDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthTargetDesc.SampleDesc.Count = desc.mMultiSampleCount;
    depthTargetDesc.SampleDesc.Quality = desc.mMultiSampleQuality;
    depthTargetDesc.Usage = D3D11_USAGE_DEFAULT;

    if (desc.mFormat == eFormat_R24G8_TYPELESS)
        depthTargetDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    else
        depthTargetDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    depthTargetDesc.CPUAccessFlags = 0;
    depthTargetDesc.MiscFlags = D3D11ResourceMiscFlag(desc.mFlags);

    pDepthTargetRaw = std::make_shared<DrawingRawDepthTarget_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), depthTargetDesc);

    auto pDepthBuffer = std::make_shared<DrawingDepthBuffer>(shared_from_this());
    pDepthBuffer->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pDepthBuffer->SetResource(pDepthTargetRaw);

    pRes = pDepthBuffer;
    return true;
}

bool DrawingDevice_D3D11::CreateBlendState(const DrawingBlendStateDesc& desc, std::shared_ptr<DrawingBlendState>& pRes)
{
    D3D11_BLEND_DESC blendDesc;
    for (uint32_t i = 0; i < MAX_TARGETS; ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable = desc.mTargets[i].mBlendEnable;
        blendDesc.RenderTarget[i].SrcBlend = D3D11Enum(desc.mTargets[i].mColorBlend.mBlendSrc);
        blendDesc.RenderTarget[i].DestBlend = D3D11Enum(desc.mTargets[i].mColorBlend.mBlendDst);
        blendDesc.RenderTarget[i].BlendOp = D3D11Enum(desc.mTargets[i].mColorBlend.mBlendOp);
        blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11Enum(desc.mTargets[i].mAlphaBlend.mBlendSrc);
        blendDesc.RenderTarget[i].DestBlendAlpha = D3D11Enum(desc.mTargets[i].mAlphaBlend.mBlendDst);
        blendDesc.RenderTarget[i].BlendOpAlpha = D3D11Enum(desc.mTargets[i].mAlphaBlend.mBlendOp);
        blendDesc.RenderTarget[i].RenderTargetWriteMask = desc.mTargets[i].mRenderTargetWriteMask;
    }
    blendDesc.AlphaToCoverageEnable = desc.mAlphaToCoverageEnable;
    blendDesc.IndependentBlendEnable = desc.mIndependentBlendEnable;

    auto pBlendState = std::make_shared<DrawingBlendState>(shared_from_this());

    std::shared_ptr<DrawingRawBlendState> pBlendStateRaw = std::make_shared<DrawingRawBlendState_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), blendDesc);
    pBlendState->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pBlendState->SetResource(pBlendStateRaw);

    pRes = pBlendState;
    return true;
}

bool DrawingDevice_D3D11::CreateDepthState(const DrawingDepthStateDesc& desc, std::shared_ptr<DrawingDepthState>& pRes)
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = desc.mDepthState.mDepthEnable;
    depthStencilDesc.DepthWriteMask = desc.mDepthState.mDepthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11Enum(desc.mDepthState.mDepthFunc);
    depthStencilDesc.StencilEnable = desc.mStencilState.mStencilEnable;
    depthStencilDesc.StencilReadMask = desc.mStencilState.mStencilReadMask;
    depthStencilDesc.StencilWriteMask = desc.mStencilState.mStencilWriteMask;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11Enum(desc.mStencilState.mFrontFace.mStencilFailOp);
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11Enum(desc.mStencilState.mFrontFace.mStencilDepthFailOp);
    depthStencilDesc.FrontFace.StencilPassOp = D3D11Enum(desc.mStencilState.mFrontFace.mStencilPassOp);
    depthStencilDesc.FrontFace.StencilFunc = D3D11Enum(desc.mStencilState.mFrontFace.mStencilFunc);
    depthStencilDesc.BackFace.StencilFailOp = D3D11Enum(desc.mStencilState.mBackFace.mStencilFailOp);
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11Enum(desc.mStencilState.mBackFace.mStencilDepthFailOp);
    depthStencilDesc.BackFace.StencilPassOp = D3D11Enum(desc.mStencilState.mBackFace.mStencilPassOp);
    depthStencilDesc.BackFace.StencilFunc = D3D11Enum(desc.mStencilState.mBackFace.mStencilFunc);
    
    auto pDepthState = std::make_shared<DrawingDepthState>(shared_from_this());

    std::shared_ptr<DrawingRawDepthState> pDepthStateRaw = std::make_shared<DrawingRawDepthState_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), depthStencilDesc);
    pDepthState->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pDepthState->SetResource(pDepthStateRaw);

    pRes = pDepthState;
    return true;
}

bool DrawingDevice_D3D11::CreateRasterState(const DrawingRasterStateDesc& desc, std::shared_ptr<DrawingRasterState>& pRes)
{
    D3D11_RASTERIZER_DESC rasterizerDesc;
    rasterizerDesc.FillMode = D3D11Enum(desc.mFillMode);
    rasterizerDesc.CullMode = D3D11Enum(desc.mCullMode);
    rasterizerDesc.FrontCounterClockwise = desc.mFrontCounterClockwise;
    rasterizerDesc.DepthBias = desc.mDepthBias;
    rasterizerDesc.DepthBiasClamp = desc.mDepthBiasClamp;
    rasterizerDesc.SlopeScaledDepthBias = desc.mSlopeScaledDepthBias;
    rasterizerDesc.DepthClipEnable = desc.mDepthClipEnable;
    rasterizerDesc.ScissorEnable = desc.mScissorEnable;
    rasterizerDesc.MultisampleEnable = desc.mMultisampleEnable;
    rasterizerDesc.AntialiasedLineEnable = desc.mAntialiasedLineEnable;

    auto pRasterState = std::make_shared<DrawingRasterState>(shared_from_this());

    std::shared_ptr<DrawingRawRasterState> pRasterStateRaw = std::make_shared<DrawingRawRasterState_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), rasterizerDesc);
    pRasterState->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pRasterState->SetResource(pRasterStateRaw);

    pRes = pRasterState;
    return true;
}

bool DrawingDevice_D3D11::CreateSamplerState(const DrawingSamplerStateDesc& desc, std::shared_ptr<DrawingSamplerState>& pRes)
{
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11Enum(desc.mSamplerMode, desc.mMinFilter, desc.mMagFilter, desc.mMipFilter);
    samplerDesc.AddressU = D3D11Enum(desc.mAddressU);
    samplerDesc.AddressV = D3D11Enum(desc.mAddressV);
    samplerDesc.AddressW = D3D11Enum(desc.mAddressW);
    samplerDesc.MipLODBias = desc.mMipLODBias;
    samplerDesc.MaxAnisotropy = desc.mMaxAnisotropy;
    samplerDesc.ComparisonFunc = D3D11Enum(desc.mComparisonFunc);
    samplerDesc.BorderColor[0] = desc.mBorderColor[0];
    samplerDesc.BorderColor[1] = desc.mBorderColor[1];
    samplerDesc.BorderColor[2] = desc.mBorderColor[2];
    samplerDesc.BorderColor[3] = desc.mBorderColor[3];
    samplerDesc.MinLOD = desc.mMinLOD;
    samplerDesc.MaxLOD = desc.mMaxLOD;

    auto pSamplerState = std::make_shared<DrawingSamplerState>(shared_from_this());

    std::shared_ptr<DrawingRawSamplerState> pSamplerStateRaw = std::make_shared<DrawingRawSamplerState_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), samplerDesc);
    pSamplerState->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pSamplerState->SetResource(pSamplerStateRaw);

    pRes = pSamplerState;
    return true;
}

bool DrawingDevice_D3D11::CreateEffectFromFile(const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes)
{
    assert(desc.mProgramType == eProgram_File);

    std::ifstream fstream(desc.mpSourceName->c_str(), std::ios::binary);
    if (!fstream)
        return false;

    std::string content((std::istreambuf_iterator<char>(fstream)), std::istreambuf_iterator<char>());

    return DoCreateEffect(desc, content.data(), (uint32_t)content.length(), pRes);
}

bool DrawingDevice_D3D11::CreateEffectFromString(const std::string& str, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes)
{
    assert(desc.mProgramType == eProgram_String);

    return DoCreateEffect(desc, str.data(), (uint32_t)str.length(), pRes);
}

bool DrawingDevice_D3D11::CreateEffectFromBuffer(const void* pData, uint32_t length, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes)
{
    assert(desc.mProgramType != eProgram_File);

    return DoCreateEffect(desc, pData, length, pRes);
}

bool DrawingDevice_D3D11::CreateEffectFromShader(const DrawingEffectDesc& desc, std::shared_ptr<DrawingVertexShader> pVSShader, std::shared_ptr<DrawingPixelShader> pPSShader, std::shared_ptr<DrawingEffect>& pRes)
{
    assert(desc.mProgramType == eProgram_Shader);

    auto pVSShaderRaw = std::dynamic_pointer_cast<DrawingRawVertexShader_D3D11>(pVSShader->GetResource());
    assert(pVSShaderRaw != nullptr);
    auto pPSShaderRaw = std::dynamic_pointer_cast<DrawingRawPixelShader_D3D11>(pPSShader->GetResource());
    assert(pPSShaderRaw != nullptr);

    auto pEffectRaw = std::make_shared<DrawingRawShaderEffect_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), desc.mpName, pVSShaderRaw, pPSShaderRaw);

    auto pEffect = std::make_shared<DrawingEffect>(shared_from_this());
    pEffect->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pEffect->SetResource(pEffectRaw);

    pRes = pEffect;
    return true;
}

bool DrawingDevice_D3D11::CreateVertexShaderFromFile(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    assert(desc.mProgramType == eProgram_File);

    std::ifstream fstream(desc.mpSourceName->c_str(), std::ios::binary);
    if (!fstream)
        return false;

    std::string content((std::istreambuf_iterator<char>(fstream)), std::istreambuf_iterator<char>());

    return DoCreateVertexShader(desc, content.data(), (uint32_t)content.length(), pRes);
}

bool DrawingDevice_D3D11::CreateVertexShaderFromString(const std::string& str, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    assert(desc.mProgramType == eProgram_String);

    return DoCreateVertexShader(desc, str.data(), (uint32_t)str.length(), pRes);
}

bool DrawingDevice_D3D11::CreateVertexShaderFromBuffer(const void* pData, uint32_t length, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    assert(desc.mProgramType == eProgram_File);

    return DoCreateVertexShader(desc, pData, length, pRes);
}

bool DrawingDevice_D3D11::CreatePixelShaderFromFile(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    assert(desc.mProgramType == eProgram_File);

    std::ifstream fstream(desc.mpSourceName->c_str(), std::ios::binary);
    if (!fstream)
        return false;

    std::string content((std::istreambuf_iterator<char>(fstream)), std::istreambuf_iterator<char>());

    return DoCreatePixelShader(desc, content.data(), (uint32_t)content.length(), pRes);
}

bool DrawingDevice_D3D11::CreatePixelShaderFromString(const std::string& str, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    assert(desc.mProgramType == eProgram_String);

    return DoCreatePixelShader(desc, str.data(), (uint32_t)str.length(), pRes);
}

bool DrawingDevice_D3D11::CreatePixelShaderFromBuffer(const void* pData, uint32_t length, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    assert(desc.mProgramType == eProgram_File);

    return DoCreatePixelShader(desc, pData, length, pRes);
}

void DrawingDevice_D3D11::ClearTarget(std::shared_ptr<DrawingTarget> pTarget, const float4& color)
{
    auto pTargetRaw = std::dynamic_pointer_cast<DrawingRawFragmentTarget_D3D11>(pTarget->GetResource());
    assert(pTargetRaw != nullptr);

    m_pDeviceContext->ClearRenderTargetView(pTargetRaw->GetRenderTargetView().get(), color.mData);
}

void DrawingDevice_D3D11::ClearDepthBuffer(std::shared_ptr<DrawingDepthBuffer> pDepthBuffer, float depth, uint8_t stencil, uint32_t flag)
{
    auto pTargetRaw = std::dynamic_pointer_cast<DrawingRawDepthTarget_D3D11>(pDepthBuffer->GetResource());
    assert(pTargetRaw != nullptr);

    uint32_t clearFlags = 0;
    if (flag & eClear_Depth)
        clearFlags |= D3D11_CLEAR_DEPTH;
    if (flag & eClear_Stencil)
        clearFlags |= D3D11_CLEAR_STENCIL;

    m_pDeviceContext->ClearDepthStencilView(pTargetRaw->GetDepthStencilView().get(), clearFlags, depth, stencil);
}

void DrawingDevice_D3D11::SetVertexFormat(std::shared_ptr<DrawingVertexFormat> pFormat)
{
    if (pFormat == nullptr)
    {
        m_pDeviceContext->IASetInputLayout(nullptr);
    }
    else
    {
        auto pVertexFormatRaw = std::dynamic_pointer_cast<DrawingRawVertexFormat_D3D11>(pFormat->GetResource());
        m_pDeviceContext->IASetInputLayout(pVertexFormatRaw->Get().get());
    }
}

void DrawingDevice_D3D11::SetVertexBuffer(std::shared_ptr<DrawingVertexBuffer> pVB[], uint32_t count)
{
    std::shared_ptr<DrawingRawVertexBuffer_D3D11> pVertexBuffersRaw[MAX_VERTEX_STREAM] = { nullptr };
    for (uint32_t index = 0; index < count; ++index)
    {
        if (pVB[index] != nullptr)
            pVertexBuffersRaw[index] = std::dynamic_pointer_cast<DrawingRawVertexBuffer_D3D11>(pVB[index]->GetResource());
        else
            pVertexBuffersRaw[index] = nullptr;
    }

    assert(count < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
    ID3D11Buffer* pVertexBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
    uint32_t strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
    uint32_t offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };

    for (uint32_t index = 0; index < count; ++index)
    {
        if (pVertexBuffersRaw[index] != nullptr)
        {
            pVertexBuffer[index] = pVertexBuffersRaw[index]->GetBuffer().get();
            strides[index] = pVertexBuffersRaw[index]->GetStride();
            offsets[index] = pVertexBuffersRaw[index]->GetStartOffset();
        }
        else
        {
            pVertexBuffer[index] = nullptr;
            strides[index] = 0;
            offsets[index] = 0;
        }
    }

    m_pDeviceContext->IASetVertexBuffers(0, count, pVertexBuffer, strides, offsets);
}

void DrawingDevice_D3D11::SetIndexBuffer(std::shared_ptr<DrawingIndexBuffer> pIB)
{
    if (pIB != nullptr)
    {
        auto pIndexBufferRaw = std::dynamic_pointer_cast<DrawingRawIndexBuffer_D3D11>(pIB->GetResource());
        assert(pIndexBufferRaw != nullptr);
        m_pDeviceContext->IASetIndexBuffer(pIndexBufferRaw->GetBuffer().get(), pIndexBufferRaw->GetBufferFormat(), pIndexBufferRaw->GetStartOffset());
    }
    else
        m_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
}

void DrawingDevice_D3D11::SetBlendState(std::shared_ptr<DrawingBlendState> pBlend, float4 blendFactor, uint32_t sampleMask)
{
    if (pBlend != nullptr)
    {
        auto pBlendState = std::dynamic_pointer_cast<DrawingRawBlendState_D3D11>(pBlend->GetResource()); 
        assert(pBlendState != nullptr);
        m_pDeviceContext->OMSetBlendState(pBlendState->GetState().get(), blendFactor.mData, sampleMask);
    }
    else
        m_pDeviceContext->OMSetBlendState(nullptr, float4(1.0f).mData, 0xffffffff);
}

void DrawingDevice_D3D11::SetDepthState(std::shared_ptr<DrawingDepthState> pDepth, uint32_t stencilRef)
{
    if (pDepth != nullptr)
    {
        auto pDepthState = std::dynamic_pointer_cast<DrawingRawDepthState_D3D11>(pDepth->GetResource()); 
        assert(pDepthState != nullptr);
        m_pDeviceContext->OMSetDepthStencilState(pDepthState->GetState().get(), stencilRef);
    }
    else
        m_pDeviceContext->OMSetDepthStencilState(nullptr, 1U);
}

void DrawingDevice_D3D11::SetRasterState(std::shared_ptr<DrawingRasterState> pRaster)
{
    if (pRaster != nullptr)
    {
        auto pRasterState = std::dynamic_pointer_cast<DrawingRawRasterState_D3D11>(pRaster->GetResource()); 
        assert(pRasterState != nullptr);
        m_pDeviceContext->RSSetState(pRasterState->GetState().get());
    }
    else
        m_pDeviceContext->RSSetState(nullptr);
}

void DrawingDevice_D3D11::PushBlendState()
{
    m_blendStates.push(GetBlendState());
}

void DrawingDevice_D3D11::PopBlendState()
{
    assert(!m_blendStates.empty());

    auto state = m_blendStates.top();
    SetBlendState(state);
    m_blendStates.pop();
}

void DrawingDevice_D3D11::PushDepthState()
{
    m_depthStates.push(GetDepthState());
}

void DrawingDevice_D3D11::PopDepthState()
{
    assert(!m_depthStates.empty());

    auto state = m_depthStates.top();
    SetDepthState(state);
    m_depthStates.pop();
}

void DrawingDevice_D3D11::PushRasterState()
{
    m_rasterStates.push(GetRasterState());
}

void DrawingDevice_D3D11::PopRasterState()
{
    assert(!m_rasterStates.empty());

    auto state = m_rasterStates.top();
    SetRasterState(state);
    m_rasterStates.pop();
}

void DrawingDevice_D3D11::SetViewport(Box2* vp)
{
    if (vp == nullptr)
        m_pDeviceContext->RSSetViewports(1, nullptr);
    else
    {
        const auto& topLeft = vp->mMin;
        const D3D11_VIEWPORT viewport{ topLeft.x, topLeft.y, vp->Width(), vp->Height(), 0, 1.f };
        m_pDeviceContext->RSSetViewports(1, &viewport);
    }
}

void DrawingDevice_D3D11::SetBlendState(const std::shared_ptr<DrawingRawBlendState_D3D11> pBlendState, const float4& blendFactor, uint32_t sampleMask)
{
    ID3D11BlendState* pRaw = pBlendState != nullptr ? pBlendState->GetState().get() : nullptr;
    m_pDeviceContext->OMSetBlendState(pRaw, blendFactor.mData, sampleMask);
}

void DrawingDevice_D3D11::SetDepthState(const std::shared_ptr<DrawingRawDepthState_D3D11> pDepthState, uint32_t stencilRef)
{
    ID3D11DepthStencilState* pRaw = pDepthState != nullptr ? pDepthState->GetState().get() : nullptr;
    m_pDeviceContext->OMSetDepthStencilState(pRaw, stencilRef);
}

void DrawingDevice_D3D11::SetRasterState(const std::shared_ptr<DrawingRawRasterState_D3D11> pRasterState)
{
    ID3D11RasterizerState* pRaw = pRasterState != nullptr ? pRasterState->GetState().get() : nullptr;
    m_pDeviceContext->RSSetState(pRaw);
}

void DrawingDevice_D3D11::SetBlendState(const BlendState& state)
{
    auto blendFactor = std::get<0>(state);
    auto sampleMask = std::get<1>(state);
    auto pBlendState = std::get<2>(state);

    SetBlendState(pBlendState, blendFactor, sampleMask);
}

void DrawingDevice_D3D11::SetDepthState(const DepthState& state)
{
    auto stencilRef = std::get<0>(state);
    auto pDepthState = std::get<1>(state);

    SetDepthState(pDepthState, stencilRef);
}

void DrawingDevice_D3D11::SetRasterState(const RasterState& state)
{
    auto pRasterState = std::get<0>(state);

    SetRasterState(pRasterState);
}

DrawingDevice_D3D11::BlendState DrawingDevice_D3D11::GetBlendState()
{
    ID3D11BlendState* pRaw;
    float4 blendFactor { 1.0f };
    uint32_t sampleMask { 0xffffffff };

    m_pDeviceContext->OMGetBlendState(&pRaw, blendFactor.mData, &sampleMask);

    return std::make_tuple(blendFactor, sampleMask, std::make_shared<DrawingRawBlendState_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), std::shared_ptr<ID3D11BlendState>(pRaw)));
}

DrawingDevice_D3D11::DepthState DrawingDevice_D3D11::GetDepthState()
{
    ID3D11DepthStencilState* pRaw;
    uint32_t stencilRef { 1 };

    m_pDeviceContext->OMGetDepthStencilState(&pRaw, &stencilRef);
    return std::make_tuple(stencilRef, std::make_shared<DrawingRawDepthState_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), std::shared_ptr<ID3D11DepthStencilState>(pRaw)));
}

DrawingDevice_D3D11::RasterState DrawingDevice_D3D11::GetRasterState()
{
    ID3D11RasterizerState* pRaw;

    m_pDeviceContext->RSGetState(&pRaw);
    return std::make_tuple(std::make_shared<DrawingRawRasterState_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), std::shared_ptr<ID3D11RasterizerState>(pRaw)));
}

void DrawingDevice_D3D11::SetTargets(std::shared_ptr<DrawingTarget> pTarget[], uint32_t maxTargets, std::shared_ptr<DrawingDepthBuffer> pDepthBuffer, std::shared_ptr<DrawingRWBuffer> pRWBuffer[], uint32_t maxRWBuffers)
{
    assert(maxTargets <= MAX_RENDER_TARGET_COUNT);
    assert(maxRWBuffers <= MAX_UAV_SLOT_COUNT);

    auto pDepthBufferRaw = pDepthBuffer != nullptr ? std::dynamic_pointer_cast<DrawingRawDepthTarget_D3D11>(pDepthBuffer->GetResource()) : nullptr;
    std::shared_ptr<DrawingRawFragmentTarget_D3D11> pTargetsRaw[MAX_RENDER_TARGET_COUNT] = { nullptr };

    if (maxTargets != 0)
    {
        std::transform(pTarget, pTarget + maxTargets, pTargetsRaw,
        [](std::shared_ptr<DrawingTarget> target) {
            return target != nullptr ? std::dynamic_pointer_cast<DrawingRawFragmentTarget_D3D11>(target->GetResource()) : nullptr;
        });
    }

    if (maxRWBuffers != 0)
    {
        std::shared_ptr<DrawingRawRWBuffer_D3D11> pBuffersRaw[MAX_UAV_SLOT_COUNT] = { nullptr };
        std::transform(pRWBuffer, pRWBuffer + maxRWBuffers, pBuffersRaw,
        [](std::shared_ptr<DrawingRWBuffer> rwBuffer) {
            return rwBuffer != nullptr ? std::dynamic_pointer_cast<DrawingRawRWBuffer_D3D11>(rwBuffer->GetResource()) : nullptr;
        });
 
        ID3D11DepthStencilView* pDepthStencilView = pDepthBufferRaw != nullptr ? pDepthBufferRaw->GetDepthStencilView().get() : nullptr;

        ID3D11RenderTargetView* pRenderTargetView[MAX_RENDER_TARGET_COUNT] = { nullptr };
        std::transform(pTargetsRaw, pTargetsRaw + maxTargets, pRenderTargetView,
        [](std::shared_ptr<DrawingRawFragmentTarget_D3D11> target) {
            return target != nullptr ? target->GetRenderTargetView().get() : nullptr; 
        });

        ID3D11UnorderedAccessView* pUAView[MAX_UAV_SLOT_COUNT] = { nullptr };
        std::transform(pBuffersRaw, pBuffersRaw + maxRWBuffers, pUAView,
        [](std::shared_ptr<DrawingRawRWBuffer_D3D11> target) {
            return target != nullptr ? target->GetUnorderedAccessView().get() : nullptr;
        });

        m_pDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(maxTargets, pRenderTargetView, pDepthStencilView, maxTargets, maxRWBuffers, pUAView, nullptr);
    }
    else
    {
        ID3D11DepthStencilView* pDepthStencilView = pDepthBufferRaw != nullptr ? pDepthBufferRaw->GetDepthStencilView().get() : nullptr;

        ID3D11RenderTargetView* pRenderTargetView[MAX_RENDER_TARGET_COUNT] = { nullptr };
        std::transform(pTargetsRaw, pTargetsRaw + maxTargets, pRenderTargetView,
        [](std::shared_ptr<DrawingRawFragmentTarget_D3D11> target) {
            return target != nullptr ? target->GetRenderTargetView().get() : nullptr; 
        });

        m_pDeviceContext->OMSetRenderTargets(maxTargets, pRenderTargetView, pDepthStencilView);
    }
}

bool DrawingDevice_D3D11::UpdateEffectParameter(std::shared_ptr<DrawingParameter> pParam, std::shared_ptr<DrawingEffect> pEffect)
{
    assert(pEffect != nullptr);
    auto pRawEffect = std::dynamic_pointer_cast<DrawingRawEffect_D3D11>(pEffect->GetResource());
    assert(pRawEffect != nullptr);

    pRawEffect->UpdateParameter(pParam);

    return true;
}

bool DrawingDevice_D3D11::UpdateEffectTexture(std::shared_ptr<DrawingTexture> pTex, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    assert(pTex != nullptr);
    assert(pEffect != nullptr);

    auto pRawEffect = std::dynamic_pointer_cast<DrawingRawEffect_D3D11>(pEffect->GetResource());
    assert(pRawEffect != nullptr);

    auto pRawTex = std::dynamic_pointer_cast<DrawingRawTexture_D3D11>(pTex->GetResource());
    assert(pRawTex != nullptr);

    auto pParamSet = pRawEffect->GetParameterSet();

    int32_t index = pParamSet.IndexOfName(pName);
    if (index < 0)
        return false;

    auto pParam = pParamSet[index];
    if (pParam == nullptr)
        return false;

    pParam->AsTexture(pRawTex.get());

    return true;
}

bool DrawingDevice_D3D11::UpdateEffectBuffer(std::shared_ptr<DrawingTexBuffer> pBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

bool DrawingDevice_D3D11::UpdateEffectSampler(std::shared_ptr<DrawingSamplerState> pSampler, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    assert(pEffect != nullptr);
    assert(pSampler != nullptr);
    assert(pName != nullptr);

    auto pRawEffect = std::dynamic_pointer_cast<DrawingRawEffect_D3D11>(pEffect->GetResource());
    assert(pRawEffect != nullptr);

    auto pRawSampler = std::dynamic_pointer_cast<DrawingRawSamplerState_D3D11>(pSampler->GetResource());
    assert(pRawSampler != nullptr);

    auto pParamSet = pRawEffect->GetParameterSet();

    int32_t index = pParamSet.IndexOfName(pName);
    if (index < 0)
        return false;

    auto pParam = pParamSet[index];
    if (pParam == nullptr)
        return false;

    pParam->AsSampler(pRawSampler.get());

    return true;
}

bool DrawingDevice_D3D11::UpdateEffectTexBuffer(std::shared_ptr<DrawingTexBuffer> pTexBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

bool DrawingDevice_D3D11::UpdateEffectRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

bool DrawingDevice_D3D11::UpdateEffectInputRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

bool DrawingDevice_D3D11::UpdateEffectOutputRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

void DrawingDevice_D3D11::BeginEffect(DrawingContext& dc, std::shared_ptr<DrawingEffect> pEffect)
{
    assert(pEffect != nullptr);

    auto pRawEffect = std::dynamic_pointer_cast<DrawingRawEffect_D3D11>(pEffect->GetResource());
    assert(pRawEffect != nullptr);

    pRawEffect->Apply();
}

void DrawingDevice_D3D11::EndEffect(DrawingContext& dc, std::shared_ptr<DrawingEffect> pEffect)
{
    assert(pEffect != nullptr);

    auto pRawEffect = std::dynamic_pointer_cast<DrawingRawEffect_D3D11>(pEffect->GetResource());
    assert(pRawEffect != nullptr);

    pRawEffect->Terminate();
}

bool DrawingDevice_D3D11::DrawPrimitive(std::shared_ptr<DrawingPrimitive> pRes)
{
    assert(pRes != nullptr);

    auto primType = D3D11Enum(pRes->GetPrimitiveType());
    auto indexCount = pRes->GetIndexCount();
    auto instanceCount = pRes->GetInstanceCount();

    m_pDeviceContext->IASetPrimitiveTopology(primType);

    if (instanceCount != 0)
    {
        if (indexCount != 0)
            m_pDeviceContext->DrawIndexedInstanced(indexCount, instanceCount, pRes->GetIndexOffset(), 0, pRes->GetInstanceOffset());
        else
            m_pDeviceContext->DrawInstanced(pRes->GetVertexCount(), pRes->GetInstanceCount(), pRes->GetVertexOffset(), pRes->GetInstanceOffset());
    }
    else
    {
        if (indexCount != 0)
            m_pDeviceContext->DrawIndexed(indexCount, pRes->GetIndexOffset(), 0);
        else
            m_pDeviceContext->Draw(pRes->GetVertexCount(), pRes->GetVertexOffset());
    }

    return true;
}

bool DrawingDevice_D3D11::Present(const std::shared_ptr<DrawingTarget> pTarget, uint32_t syncInterval)
{
    auto pSwapChainRaw = std::static_pointer_cast<DrawingRawSwapChain_D3D11>(pTarget->GetResource());
    assert(pSwapChainRaw != nullptr);

    HRESULT hr = pSwapChainRaw->Present(syncInterval);
    if (!SUCCEEDED(hr))
        return false;

    return true;
}

void* DrawingDevice_D3D11::Map(std::shared_ptr<DrawingResource> pRes, uint32_t subID, EDrawingAccessType flag, uint32_t& rowPitch, uint32_t& slicePitch, uint32_t offset, uint32_t sizeInBytes)
{
    assert(pRes != nullptr);

    if (subID < 0)
        subID = 0;

    switch(pRes->GetType())
    {
    case eResource_Texture:
        return MapResource<DrawingRawTexture_D3D11, DrawingTexture>(pRes, subID, flag, rowPitch, slicePitch);

    case eResource_Vertex_Buffer:
        return MapResource<DrawingRawVertexBuffer_D3D11, DrawingVertexBuffer>(pRes, subID, flag, rowPitch, slicePitch);

    case eResource_Index_Buffer:
        return MapResource<DrawingRawIndexBuffer_D3D11, DrawingIndexBuffer>(pRes, subID, flag, rowPitch, slicePitch);
    }

    return nullptr;
}

void DrawingDevice_D3D11::UnMap(std::shared_ptr<DrawingResource> pRes, uint32_t subID)
{
    assert(pRes != nullptr);

    if (subID < 0)
        subID = 0;

    switch(pRes->GetType())
    {
    case eResource_Texture:
        return UnMapResource<DrawingRawTexture_D3D11, DrawingTexture>(pRes, subID);

    case eResource_Vertex_Buffer:
        return UnMapResource<DrawingRawVertexBuffer_D3D11, DrawingVertexBuffer>(pRes, subID);

    case eResource_Index_Buffer:
        return UnMapResource<DrawingRawIndexBuffer_D3D11, DrawingIndexBuffer>(pRes, subID);
    }
}

bool DrawingDevice_D3D11::CopyBuffer(std::shared_ptr<DrawingResource> pDstRes, std::shared_ptr<DrawingResource> pSrcRes, uint32_t dstSubID, uint32_t srcSubID, uint32_t dstStartInBytes, uint32_t srcStartInBytes, uint32_t sizeInBytes)
{
    return true;
}

bool DrawingDevice_D3D11::CopyTexture(std::shared_ptr<DrawingResource> pDstRes, std::shared_ptr<DrawingResource> pSrcRes, uint32_t dstSubID, uint32_t srcSubID, const int3& srcMin, const int3& srcMax, const int3& dstOrigin)
{
    assert(pDstRes != nullptr && pSrcRes != nullptr);
    assert(pDstRes != pSrcRes);

    switch(pDstRes->GetType())
    {
        case eResource_Target:
        {
            auto pDstTarget = std::dynamic_pointer_cast<DrawingTarget>(pDstRes);
            switch(pSrcRes->GetType())
            {
                case eResource_Target:
                {
                    auto pSrcTarget = std::dynamic_pointer_cast<DrawingTarget>(pSrcRes);
                    return CopyTextureData<DrawingRawTarget, DrawingRawTarget, DrawingRawTarget_D3D11, DrawingRawTarget_D3D11>(pDstTarget.get(), dstSubID, pSrcTarget.get(), srcSubID, srcMin, srcMax, dstOrigin);
                }
                case eResource_Texture:
                {
                    auto pSrcTex = std::dynamic_pointer_cast<DrawingTexture>(pSrcRes);
                    return CopyTextureData<DrawingRawTarget, DrawingRawTexture, DrawingRawTarget_D3D11, DrawingRawTexture_D3D11>(pDstTarget.get(), dstSubID, pSrcTex.get(), srcSubID, srcMin, srcMax, dstOrigin);
                }
                default:
                    return false;
            }
        }
        case eResource_Texture:
        {
            auto pDstTex = std::dynamic_pointer_cast<DrawingTexture>(pDstRes);
            switch(pSrcRes->GetType())
            {
                case eResource_Target:
                {
                    auto pSrcTarget = std::dynamic_pointer_cast<DrawingTarget>(pSrcRes);
                    return CopyTextureData<DrawingRawTexture, DrawingRawTarget, DrawingRawTexture_D3D11, DrawingRawTarget_D3D11>(pDstTex.get(), dstSubID, pSrcTarget.get(), srcSubID, srcMin, srcMax, dstOrigin);
                }
                case eResource_Texture:
                {
                    auto pSrcTex = std::dynamic_pointer_cast<DrawingTexture>(pSrcRes);
                    return CopyTextureData<DrawingRawTexture, DrawingRawTexture, DrawingRawTexture_D3D11, DrawingRawTexture_D3D11>(pDstTex.get(), dstSubID, pSrcTex.get(), srcSubID, srcMin, srcMax, dstOrigin);
                }
                default:
                    return false;
            }
        }
        default:
            return false;
    }
}

void DrawingDevice_D3D11::Flush()
{
    m_pDeviceContext->Flush();
}

uint32_t DrawingDevice_D3D11::FormatBytes(EDrawingFormatType type)
{
    return D3D11FormatBytes(type);
}

std::shared_ptr<ID3D11Device> DrawingDevice_D3D11::GetDevice() const
{
    return m_pDevice;
}

std::shared_ptr<ID3D11DeviceContext> DrawingDevice_D3D11::GetDeviceContext() const
{
    return m_pDeviceContext;
}

std::shared_ptr<IDXGIFactory> DrawingDevice_D3D11::GetDXGIFactory() const
{
    return m_pDXGIFactory;
}

bool DrawingDevice_D3D11::DoCreateEffect(const DrawingEffectDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingEffect>& pRes)
{
    auto pEffectName = desc.mpName;
    auto pTechName = static_cast<const DrawingGeneralEffectDesc*>(&desc)->mpTechName;

    auto pEffectRaw = CreateEffectFromMemory(pEffectName, pTechName, pData, size);
    if (pEffectRaw == nullptr)
        return false;

    auto pEffect = std::make_shared<DrawingEffect>(shared_from_this());
    pEffect->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pEffect->SetResource(pEffectRaw);

    pRes = pEffect;
    return true;
}

bool DrawingDevice_D3D11::DoCreateVertexShader(const DrawingVertexShaderDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingVertexShader>& pRes)
{
    auto pVertexShaderRaw = CreateVertexShaderFromBuffer(desc.mpName, desc.mpEntryName, desc.mpSourceName, pData, size);
    if (pVertexShaderRaw == nullptr)
        return false;

    auto pVertexShader = std::make_shared<DrawingVertexShader>(shared_from_this());
    pVertexShader->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pVertexShader->SetResource(pVertexShaderRaw);
    
    pRes = pVertexShader;
    return true;
}

bool DrawingDevice_D3D11::DoCreatePixelShader(const DrawingPixelShaderDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingPixelShader>& pRes)
{
    auto pPixelShaderRaw = CreatePixelShaderFromBuffer(desc.mpName, desc.mpEntryName, desc.mpSourceName, pData, size);
    if (pPixelShaderRaw == nullptr)
        return false;

    auto pPixelShader = std::make_shared<DrawingPixelShader>(shared_from_this());
    pPixelShader->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pPixelShader->SetResource(pPixelShaderRaw);
    
    pRes = pPixelShader;
    return true;
}

std::shared_ptr<DrawingRawFxEffect_D3D11> DrawingDevice_D3D11::CreateEffectFromMemory(std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pTechName, const void* pData, uint32_t size)
{
    ID3DX11Effect* pD3D11Effect = nullptr;
    HRESULT hr = D3DX11CreateEffectFromMemory(pData, size, 0, m_pDevice.get(), &pD3D11Effect);

    if (FAILED(hr) || (pD3D11Effect == nullptr))
    {
        DWORD flags = 0;

        ID3DBlob* pShader;
        ID3DBlob* pError;

#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG;
        flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        hr = D3DCompile(pData, size, nullptr, nullptr, nullptr, "", "fx_5_0", flags, 0, &pShader, &pError);
        if (SUCCEEDED(hr) && (pShader != nullptr))
            hr = D3DX11CreateEffectFromMemory(pShader->GetBufferPointer(), pShader->GetBufferSize(), 0, m_pDevice.get(), &pD3D11Effect);
        else
            return nullptr;
    }

    if (SUCCEEDED(hr) && (pD3D11Effect != nullptr))
        return std::make_shared<DrawingRawFxEffect_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), pEffectName, pTechName, std::shared_ptr<ID3DX11Effect>(pD3D11Effect, D3D11Releaser<ID3DX11Effect>));
    else
        return nullptr;
}

std::shared_ptr<DrawingRawVertexShader_D3D11> DrawingDevice_D3D11::CreateVertexShaderFromBuffer(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const void* pData, uint32_t size)
{
    assert(pName != nullptr && pData != nullptr);
    auto pVertexShader = CreateVertexShaderFromBlob(pName, pData, size);

    if (pVertexShader != nullptr)
        return pVertexShader;

    assert(pEntryName != nullptr);
    return CreateVertexShaderFromString(pName, pEntryName, pSourceName, static_cast<const char*>(pData), size);
}

std::shared_ptr<DrawingRawPixelShader_D3D11> DrawingDevice_D3D11::CreatePixelShaderFromBuffer(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const void* pData, uint32_t size)
{
    assert(pName != nullptr && pData != nullptr);
    auto pPixelShader = CreatePixelShaderFromBlob(pName, pData, size);

    if (pPixelShader != nullptr)
        return pPixelShader;

    assert(pEntryName != nullptr);
    return CreatePixelShaderFromString(pName, pEntryName, pSourceName, static_cast<const char*>(pData), size);
}

std::shared_ptr<DrawingRawVertexShader_D3D11> DrawingDevice_D3D11::CreateVertexShaderFromBlob(std::shared_ptr<std::string> pName, const void* pShaderByteCode, uint32_t length)
{
    assert(pName != nullptr && pShaderByteCode != nullptr);

    ID3D11VertexShader* pVertexShader;
    ID3D11ShaderReflection* pReflection;

    HRESULT hr = m_pDevice->CreateVertexShader(pShaderByteCode, length, nullptr, &pVertexShader);
    if (!SUCCEEDED(hr))
        return nullptr;

    hr = D3DReflect(pShaderByteCode, length, IID_ID3D11ShaderReflection, (void**)&pReflection);
    if (!SUCCEEDED(hr))
        return nullptr;

    return std::make_shared<DrawingRawVertexShader_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), pName, std::shared_ptr<ID3D11ShaderReflection>(pReflection, D3D11Releaser<ID3D11ShaderReflection>), std::shared_ptr<ID3D11VertexShader>(pVertexShader, D3D11Releaser<ID3D11VertexShader>));
}

std::shared_ptr<DrawingRawPixelShader_D3D11> DrawingDevice_D3D11::CreatePixelShaderFromBlob(std::shared_ptr<std::string> pName, const void* pShaderByteCode, uint32_t length)
{
    assert(pName != nullptr && pShaderByteCode != nullptr);

    ID3D11PixelShader* pPixelShader;
    ID3D11ShaderReflection* pReflection;

    HRESULT hr = m_pDevice->CreatePixelShader(pShaderByteCode, length, nullptr, &pPixelShader);
    if (!SUCCEEDED(hr))
        return nullptr;

    hr = D3DReflect(pShaderByteCode, length, IID_ID3D11ShaderReflection, (void**)&pReflection);
    if (!SUCCEEDED(hr))

        return nullptr;
    return std::make_shared<DrawingRawPixelShader_D3D11>(std::static_pointer_cast<DrawingDevice_D3D11>(shared_from_this()), pName, std::shared_ptr<ID3D11ShaderReflection>(pReflection, D3D11Releaser<ID3D11ShaderReflection>), std::shared_ptr<ID3D11PixelShader>(pPixelShader, D3D11Releaser<ID3D11PixelShader>));
}

std::shared_ptr<DrawingRawVertexShader_D3D11> DrawingDevice_D3D11::CreateVertexShaderFromString(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const char* pSrc, uint32_t size)
{
    assert(pName != nullptr && pEntryName != nullptr && pSrc != nullptr);

    ID3DInclude* pInclude = (pSourceName == nullptr) ? nullptr : D3D_COMPILE_STANDARD_FILE_INCLUDE;
    DWORD flags = 0;
    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    HRESULT hr = D3DCompile(pSrc, size, pSourceName.get()->c_str(), nullptr, pInclude, pEntryName.get()->c_str(), "vs_5_0", flags, 0, &pShaderBlob, &pErrorBlob);
    if (!SUCCEEDED(hr))
    {
        auto err = pErrorBlob->GetBufferPointer();
        return nullptr;
    }

    return CreateVertexShaderFromBlob(pName, pShaderBlob->GetBufferPointer(), (uint32_t)pShaderBlob->GetBufferSize());
}

std::shared_ptr<DrawingRawPixelShader_D3D11> DrawingDevice_D3D11::CreatePixelShaderFromString(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const char* pSrc, uint32_t size)
{
    assert(pName != nullptr && pEntryName != nullptr && pSrc != nullptr);

    ID3DInclude* pInclude = (pSourceName == nullptr) ? nullptr : D3D_COMPILE_STANDARD_FILE_INCLUDE;
    DWORD flags = 0;
    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    HRESULT hr = D3DCompile(pSrc, size, pSourceName.get()->c_str(), nullptr, pInclude, pEntryName.get()->c_str(), "ps_5_0", flags, 0, &pShaderBlob, &pErrorBlob);
    if (!SUCCEEDED(hr))
    {
        auto err = pErrorBlob->GetBufferPointer();
        return nullptr;
    }

    return CreatePixelShaderFromBlob(pName, pShaderBlob->GetBufferPointer(), (uint32_t)pShaderBlob->GetBufferSize());
}

template <typename T, typename U, typename SubT, typename SubU>
bool DrawingDevice_D3D11::CopyTextureData(DrawingResourceWrapper<T>* pDstRes, uint32_t dstSubID, DrawingResourceWrapper<U>* pSrcRes, uint32_t srcSubID, const int3& srcMin, const int3& srcMax, const int3& dstOrigin)
{
    assert(pDstRes != nullptr && pSrcRes != nullptr);

    T* pDstRawRes = static_cast<T*>(pDstRes->GetResource().get());
    assert(pDstRawRes != nullptr);
    SubT* pDstBuffer = static_cast<SubT*>(pDstRawRes);
    assert(pDstBuffer != nullptr);

    U* pSrcRawRes = static_cast<U*>(pSrcRes->GetResource().get());
    assert(pSrcRawRes != nullptr);
    SubU* pSrcBuffer = static_cast<SubU*>(pSrcRawRes);
    assert(pSrcBuffer != nullptr);

    if ((dstSubID == -1) && (srcSubID == -1))
    {
        m_pDeviceContext->CopyResource(pDstBuffer->GetBuffer().get(), pSrcBuffer->GetBuffer().get());
        return true;
    }
    assert((dstSubID != -1) && (srcSubID != -1));

    D3D11_BOX srcBox;
    srcBox.left    = srcMin.x;
    srcBox.right   = srcMax.x;
    srcBox.top     = srcMin.y;
    srcBox.bottom  = srcMax.y;
    srcBox.front   = srcMin.z;
    srcBox.back    = srcMax.z;

    m_pDeviceContext->CopySubresourceRegion(pDstBuffer->GetBuffer().get(), dstSubID, dstOrigin.x, dstOrigin.y, dstOrigin.z, pSrcBuffer->GetBuffer().get(), srcSubID, &srcBox);
    return true;
}

template<typename T, typename U>
void* DrawingDevice_D3D11::MapResource(std::shared_ptr<DrawingResource> pRes, uint32_t subID, EDrawingAccessType flag, uint32_t& rowPitch, uint32_t& slicePitch)
{
    auto pResWrap = std::dynamic_pointer_cast<U>(pRes);
    assert(pResWrap != nullptr);

    auto pMapRes = std::dynamic_pointer_cast<T>(pResWrap->GetResource());
    assert(pMapRes != nullptr);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    m_pDeviceContext->Map(pMapRes->GetBuffer().get(), subID, D3D11ResourceMapType(flag), 0, &mappedResource);

    rowPitch = mappedResource.RowPitch;
    slicePitch = mappedResource.DepthPitch;

    return mappedResource.pData;
}

template<typename T, typename U>
void DrawingDevice_D3D11::UnMapResource(std::shared_ptr<DrawingResource> pRes, uint32_t aSubID)
{
    auto pResWrap = std::dynamic_pointer_cast<U>(pRes);
    assert(pResWrap != nullptr);

    auto pMapRes = std::dynamic_pointer_cast<T>(pResWrap->GetResource());
    assert(pMapRes != nullptr);

    m_pDeviceContext->Unmap(pMapRes->GetBuffer().get(), aSubID);
}