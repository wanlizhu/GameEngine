#include <fstream>
#include <memory>
#include <sstream>
#include <d3d12shader.h>
#include <d3dcompiler.h>

#include "DrawingRawResource_D3D12.h"
#include "DrawingDevice_D3D12.h"

using namespace Engine;

DrawingDevice_D3D12::DrawingDevice_D3D12(const std::shared_ptr<ID3D12Device2> device) : DrawingDevice(), m_pDevice(device)
{
}

DrawingDevice_D3D12::~DrawingDevice_D3D12()
{
    Shutdown();
}

void DrawingDevice_D3D12::Initialize()
{
    assert(m_pDevice != nullptr);

    IDXGIFactory4* pDXGIFactoryRaw;

    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pDXGIFactoryRaw);
    assert(SUCCEEDED(hr));

    m_pDXGIFactory = std::shared_ptr<IDXGIFactory4>(pDXGIFactoryRaw, D3D12Releaser<IDXGIFactory4>);
    m_pDirectCommandManager = std::make_shared<DrawingCommandManager_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), eCommandList_Direct);
    m_pComputeCommandManager = std::make_shared<DrawingCommandManager_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), eCommandList_Compute);
    m_pCopyCommandManager = std::make_shared<DrawingCommandManager_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), eCommandList_Copy);
}

void DrawingDevice_D3D12::Shutdown()
{
}

bool DrawingDevice_D3D12::CreateVertexFormat(const DrawingVertexFormatDesc& desc, std::shared_ptr<DrawingVertexFormat>& pRes)
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc;

    for (const auto& elem : desc.m_inputElements)
    {
        D3D12_INPUT_ELEMENT_DESC input;
        input.SemanticName = elem.mpName->c_str();
        input.SemanticIndex = elem.mIndex;
        input.Format = D3D12Enum(elem.mFormat);
        input.InputSlot = elem.mSlot;
        input.AlignedByteOffset = elem.mOffset;
        input.InputSlotClass = elem.mInstanceStepRate == 0 ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        input.InstanceDataStepRate = elem.mInstanceStepRate;

        inputElementDesc.emplace_back(input);
    }

    auto pVertexFormat = std::make_shared<DrawingVertexFormat>(shared_from_this());

    std::shared_ptr<DrawingRawVertexFormat> pVertexFormatRaw = std::make_shared<DrawingRawVertexFormat_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), inputElementDesc);
    pVertexFormat->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pVertexFormat->SetResource(pVertexFormatRaw);

    pRes = pVertexFormat;

    return true;
}

bool DrawingDevice_D3D12::CreateVertexBuffer(const DrawingVertexBufferDesc& desc, std::shared_ptr<DrawingVertexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes, const void* pData, uint32_t size)
{
    if ((pData != nullptr) && (size > desc.mSizeInBytes))
        return false;

    D3D12_SUBRESOURCE_DATA subResData;
    ZeroMemory(&subResData, sizeof(D3D12_SUBRESOURCE_DATA));
    subResData.pData = pData;
    subResData.RowPitch = size;
    subResData.SlicePitch = subResData.RowPitch;

    auto pVertexBuffer = std::make_shared<DrawingVertexBuffer>(shared_from_this());
    auto commandList = m_pCopyCommandManager->GetCommandList();

    bool bStatic = desc.mUsage != eUsage_Dynamic;

    std::shared_ptr<DrawingRawVertexBuffer> pVertexBufferRaw = std::make_shared<DrawingRawVertexBuffer_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), subResData, size, desc.mStrideInBytes, bStatic);

    pVertexBuffer->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pVertexBuffer->SetResource(pVertexBufferRaw);

    pRes = pVertexBuffer;

    return true;
}

bool DrawingDevice_D3D12::CreateIndexBuffer(const DrawingIndexBufferDesc& desc, std::shared_ptr<DrawingIndexBuffer>& pRes, std::shared_ptr<DrawingResource> pRefRes, const void* pData, uint32_t size)
{
    if ((pData != nullptr) && (size > desc.mSizeInBytes))
        return false;

    D3D12_SUBRESOURCE_DATA subResData;
    ZeroMemory(&subResData, sizeof(D3D12_SUBRESOURCE_DATA));
    subResData.pData = pData;
    subResData.RowPitch = size;
    subResData.SlicePitch = subResData.RowPitch;

    auto pIndexBuffer = std::make_shared<DrawingIndexBuffer>(shared_from_this());
    auto commandList = m_pCopyCommandManager->GetCommandList();

    bool bStatic = desc.mUsage != eUsage_Dynamic;

    std::shared_ptr<DrawingRawIndexBuffer> pIndexBufferRaw = std::make_shared<DrawingRawIndexBuffer_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), subResData, size, desc.mStrideInBytes, bStatic);

    pIndexBuffer->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pIndexBuffer->SetResource(pIndexBufferRaw);

    pRes = pIndexBuffer;

    return true;
}

bool DrawingDevice_D3D12::CreateTexture(const DrawingTextureDesc& desc, std::shared_ptr<DrawingTexture>& pRes, const void* pData[], uint32_t size[], uint32_t slices)
{
    auto pTexture = std::make_shared<DrawingTexture>(shared_from_this());
    std::shared_ptr<DrawingRawTexture> pRawTexture = nullptr;

    std::vector<D3D12_SUBRESOURCE_DATA> subResData(slices);
    uint32_t dataSize = 0;
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
            D3D12_RESOURCE_DESC texture2DDesc;
            texture2DDesc = CD3DX12_RESOURCE_DESC::Tex2D(D3D12Enum(desc.mFormat), static_cast<UINT64>(desc.mWidth), static_cast<UINT>(desc.mHeight), static_cast<UINT16>(desc.mArraySize), 1);

            if (!subResData.empty())
            {
                auto mipLevels = slices / desc.mArraySize;
                for (uint32_t index = 0; index < desc.mArraySize; ++index)
                {
                    auto bytesPerRow = desc.mBytesPerRow;
                    auto bytesPerSlice = desc.mBytesPerSlice;
                    for (uint32_t level = 0; level < mipLevels; ++level)
                    {
                        auto LOD = index * level + level;
                        ZeroMemory(&subResData[LOD], sizeof(D3D12_SUBRESOURCE_DATA));
                        subResData[LOD].pData = *(pData++);
                        subResData[LOD].RowPitch = bytesPerRow;
                        dataSize += size[LOD];
                        bytesPerRow = bytesPerRow > 1U ? bytesPerRow >> 1 : 1U;
                    }
                }
            }
            pRawTexture = std::make_shared<DrawingRawTexture2D_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), texture2DDesc, subResData, dataSize);
        }
    }

    pTexture->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pTexture->SetResource(pRawTexture);

    pRes = pTexture;

    return true;
}

bool DrawingDevice_D3D12::CreateTarget(const DrawingTargetDesc& desc, std::shared_ptr<DrawingTarget>& pRes)
{
    std::shared_ptr<DrawingRawTarget> pTargetRaw = nullptr;
    if (desc.mHwnd == nullptr)
    {
    }
    else
    {
        DXGI_SWAP_CHAIN_DESC SwapChainDesc;
        SwapChainDesc.BufferDesc.Width = desc.mWidth;
        SwapChainDesc.BufferDesc.Height = desc.mHeight;
        SwapChainDesc.BufferDesc.RefreshRate.Numerator = desc.mRefreshRate;
        SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        SwapChainDesc.BufferDesc.Format = D3D12Enum(desc.mFormat);
        SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        SwapChainDesc.SampleDesc.Count = 1;
        SwapChainDesc.SampleDesc.Quality = 0;
        SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        SwapChainDesc.BufferCount = BUFFER_COUNT;
        SwapChainDesc.OutputWindow = (HWND)desc.mHwnd;
        SwapChainDesc.Windowed = true;
        SwapChainDesc.SwapEffect = D3D12Enum(desc.mSwapChain);
        SwapChainDesc.Flags = 0;

        pTargetRaw = std::make_shared<DrawingRawSwapChain_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), SwapChainDesc);
    }

    auto pTarget = std::make_shared<DrawingTarget>(shared_from_this());
    pTarget->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pTarget->SetResource(pTargetRaw);

    pRes = pTarget;
    return true;
}

bool DrawingDevice_D3D12::CreateDepthBuffer(const DrawingDepthBufferDesc& desc, std::shared_ptr<DrawingDepthBuffer>& pRes)
{
    std::shared_ptr<DrawingRawTarget> pDepthBufferRaw = nullptr;

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = D3D12Enum(desc.mFormat);
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

    pDepthBufferRaw = std::make_shared<DrawingRawDepthTarget_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), dsvDesc, desc.mWidth, desc.mHeight);

    auto pTarget = std::make_shared<DrawingDepthBuffer>(shared_from_this());
    pTarget->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pTarget->SetResource(pDepthBufferRaw);

    pRes = pTarget;
    return true;
}

bool DrawingDevice_D3D12::CreateBlendState(const DrawingBlendStateDesc& desc, std::shared_ptr<DrawingBlendState>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreateDepthState(const DrawingDepthStateDesc& desc, std::shared_ptr<DrawingDepthState>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreateRasterState(const DrawingRasterStateDesc& desc, std::shared_ptr<DrawingRasterState>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreateSamplerState(const DrawingSamplerStateDesc& desc, std::shared_ptr<DrawingSamplerState>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreateEffectFromFile(const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreateEffectFromString(const std::string& str, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes)
{
    assert(desc.mProgramType == eProgram_String);

    return DoCreateEffect(desc, str.data(), (uint32_t)str.length(), pRes);
}

bool DrawingDevice_D3D12::CreateEffectFromBuffer(const void* pData, uint32_t length, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreateEffectFromShader(const DrawingEffectDesc& desc, std::shared_ptr<DrawingVertexShader> pVSShader, std::shared_ptr<DrawingPixelShader> pPSShader, std::shared_ptr<DrawingEffect>& pRes)
{
    assert(desc.mProgramType == eProgram_Shader);

    auto pVSShaderRaw = std::dynamic_pointer_cast<DrawingRawVertexShader_D3D12>(pVSShader->GetResource());
    assert(pVSShaderRaw != nullptr);

    auto pPSShaderRaw = std::dynamic_pointer_cast<DrawingRawPixelShader_D3D12>(pPSShader->GetResource());
    assert(pPSShaderRaw != nullptr);

    auto pEffectRaw = std::make_shared<DrawingRawShaderEffect_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), desc.mpName, pVSShaderRaw, pPSShaderRaw);

    auto pEffect = std::make_shared<DrawingEffect>(shared_from_this());
    pEffect->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pEffect->SetResource(pEffectRaw);

    pRes = pEffect;
    return true;
}

bool DrawingDevice_D3D12::CreateVertexShaderFromFile(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    assert(desc.mProgramType == eProgram_File);

    std::ifstream fstream(desc.mpSourceName->c_str(), std::ios::binary);
    if (!fstream)
        return false;
        

    std::string content((std::istreambuf_iterator<char>(fstream)), std::istreambuf_iterator<char>());

    return DoCreateVertexShader(desc, content.data(), (uint32_t)content.length(), pRes);
}

bool DrawingDevice_D3D12::CreateVertexShaderFromString(const std::string& str, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreateVertexShaderFromBuffer(const void* pData, uint32_t length, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreatePixelShaderFromFile(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    assert(desc.mProgramType == eProgram_File);

    std::ifstream fstream(desc.mpSourceName->c_str(), std::ios::binary);
    if (!fstream)
        return false;

    std::string content((std::istreambuf_iterator<char>(fstream)), std::istreambuf_iterator<char>());

    return DoCreatePixelShader(desc, content.data(), (uint32_t)content.length(), pRes);
}

bool DrawingDevice_D3D12::CreatePixelShaderFromString(const std::string& str, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreatePixelShaderFromBuffer(const void* pData, uint32_t length, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::CreatePipelineState(const DrawingPipelineStateDesc& desc, const DrawingPipelineState::SubobjectResourceTable& subobjectResources, std::shared_ptr<DrawingPipelineState>& pRes)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
    std::shared_ptr<DrawingRawEffect_D3D12> pEffectRaw = nullptr;

    for (const auto& subobjectResource : subobjectResources)
    {
        switch (subobjectResource.first)
        {
            case DrawingPipelineStateDesc::ePipelineStateSubobjectType_InputLayout:
                pipelineDesc.InputLayout = std::dynamic_pointer_cast<DrawingRawVertexFormat_D3D12>(std::dynamic_pointer_cast<DrawingVertexFormat>(subobjectResource.second)->GetResource())->Get();
                break;
            case DrawingPipelineStateDesc::ePipelineStateSubobjectType_PrimitiveTopology:
                pipelineDesc.PrimitiveTopologyType = D3D12Enum(std::dynamic_pointer_cast<DrawingPrimitive>(subobjectResource.second)->GetPrimitiveType(), pipelineDesc.PrimitiveTopologyType);
                break;
            case DrawingPipelineStateDesc::ePipelineStateSubobjectType_Effect:
            {
                pEffectRaw = std::dynamic_pointer_cast<DrawingRawEffect_D3D12>(std::dynamic_pointer_cast<DrawingEffect>(subobjectResource.second)->GetResource());
                auto pShaderEffectRaw = std::dynamic_pointer_cast<DrawingRawShaderEffect_D3D12>(pEffectRaw);
                assert(pShaderEffectRaw != nullptr);
                auto pVSShaderRaw = std::dynamic_pointer_cast<DrawingRawVertexShader_D3D12>(pShaderEffectRaw->GetRawShader(DrawingRawShader::RawShader_VS));
                auto pPSShaderRaw = std::dynamic_pointer_cast<DrawingRawPixelShader_D3D12>(pShaderEffectRaw->GetRawShader(DrawingRawShader::RawShader_PS));
                auto pRootSignature = pShaderEffectRaw->GetRootSignature();
                assert(pVSShaderRaw != nullptr);
                assert(pPSShaderRaw != nullptr);
                pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(pVSShaderRaw->Get().get());
                pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(pPSShaderRaw->Get().get());
                pipelineDesc.pRootSignature = pRootSignature->GetRootSignature().get();
                break;
            }
            case DrawingPipelineStateDesc::ePipelineStateSubobjectType_BlendState:
            {
                D3D12_BLEND_DESC blendDesc = {};
                blendDesc.RenderTarget[0].BlendEnable = true;
                blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
                blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
                blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
                blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
                blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
                blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
                blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
                pipelineDesc.BlendState = CD3DX12_BLEND_DESC(blendDesc);
                pipelineDesc.SampleMask = UINT_MAX;
                break;
            }
            case DrawingPipelineStateDesc::ePipelineStateSubobjectType_RasterState:
                pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
                break;
            case DrawingPipelineStateDesc::ePipelineStateSubobjectType_DepthStencilState:
                pipelineDesc.DepthStencilState.DepthEnable = FALSE;
                pipelineDesc.DepthStencilState.StencilEnable = FALSE;
                break;
            case DrawingPipelineStateDesc::ePipelineStateSubobjectType_RenderTarget:
            {
                auto desc = std::dynamic_pointer_cast<DrawingTargetDesc>(std::dynamic_pointer_cast<DrawingTarget>(subobjectResource.second)->GetDesc());
                pipelineDesc.RTVFormats[0] = D3D12Enum(desc->mFormat);
                pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
                pipelineDesc.SampleDesc.Count = desc->mMultiSampleCount;
                pipelineDesc.SampleDesc.Quality = desc->mMultiSampleQuality;
                pipelineDesc.NumRenderTargets = 1;
                break;
            }
        }
    }

    auto pPipelineState = std::make_shared<DrawingPipelineState>(shared_from_this());
    std::shared_ptr<DrawingRawPipelineState> pPipelineStateRaw = std::make_shared<DrawingRawPipelineState_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), pEffectRaw, pipelineDesc);

    pPipelineState->SetDesc(std::shared_ptr<DrawingResourceDesc>(desc.Clone()));
    pPipelineState->SetResource(pPipelineStateRaw);

    pRes = pPipelineState;

    return true;
}

void DrawingDevice_D3D12::ClearTarget(std::shared_ptr<DrawingTarget> pTarget, const float4& color)
{
    auto pTargetRaw = std::dynamic_pointer_cast<DrawingRawFragmentTarget_D3D12>(pTarget->GetResource());
    assert(pTargetRaw != nullptr);

    auto pCommandList = m_pDirectCommandManager->GetCommandList();
    auto renderTargetViewHandle = pTargetRaw->GetRenderTargetView();

    pCommandList->TransitionBarrier(pTargetRaw->GetTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET);
    pCommandList->GetCommandList()->ClearRenderTargetView(renderTargetViewHandle, color.mData, 0, nullptr);
}

void DrawingDevice_D3D12::ClearDepthBuffer(std::shared_ptr<DrawingDepthBuffer> pDepthBuffer, float depth, uint8_t stencil, uint32_t flag)
{
}

void DrawingDevice_D3D12::SetVertexFormat(std::shared_ptr<DrawingVertexFormat> pFormat)
{
}

void DrawingDevice_D3D12::SetVertexBuffer(std::shared_ptr<DrawingVertexBuffer> pVB[], uint32_t count)
{
    std::shared_ptr<DrawingRawVertexBuffer_D3D12> pVertexBuffersRaw[MAX_VERTEX_STREAM] = { nullptr };
    for (uint32_t index = 0; index < count; ++index)
    {
        if (pVB[index] != nullptr)
            pVertexBuffersRaw[index] = std::dynamic_pointer_cast<DrawingRawVertexBuffer_D3D12>(pVB[index]->GetResource());
        else
            pVertexBuffersRaw[index] = nullptr;
    }

    assert(count < D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
    auto pCommandList = m_pDirectCommandManager->GetCommandList();

    for (uint32_t index = 0; index < count; ++index)
    {
        if (pVertexBuffersRaw[index] != nullptr)
            pCommandList->GetCommandList()->IASetVertexBuffers(index, 1, &pVertexBuffersRaw[index]->GetVertexBufferView());
    }
}

void DrawingDevice_D3D12::SetIndexBuffer(std::shared_ptr<DrawingIndexBuffer> pIB)
{
    if (pIB != nullptr)
    {
        std::shared_ptr<DrawingRawIndexBuffer_D3D12> pIndexBuffersRaw = std::dynamic_pointer_cast<DrawingRawIndexBuffer_D3D12>(pIB->GetResource());
        auto pCommandList = m_pDirectCommandManager->GetCommandList();

        pCommandList->GetCommandList()->IASetIndexBuffer(&pIndexBuffersRaw->GetIndexBufferView());
    }
}

void DrawingDevice_D3D12::SetBlendState(std::shared_ptr<DrawingBlendState> pBlend, float4 blendFactor, uint32_t sampleMask)
{
}

void DrawingDevice_D3D12::SetDepthState(std::shared_ptr<DrawingDepthState> pDepth, uint32_t stencilRef)
{
}

void DrawingDevice_D3D12::SetRasterState(std::shared_ptr<DrawingRasterState> pRaster)
{
}

void DrawingDevice_D3D12::SetPipelineState(std::shared_ptr<DrawingPipelineState> pPipelineState)
{
    if (pPipelineState != nullptr)
    {
        auto pPipelineStateRaw = std::dynamic_pointer_cast<DrawingRawPipelineState_D3D12>(pPipelineState->GetResource()); 
        assert(pPipelineStateRaw != nullptr);

        auto pCommandList = m_pDirectCommandManager->GetCommandList();
        pCommandList->GetCommandList()->SetPipelineState(pPipelineStateRaw->GetPipelineState().get());

        auto rootSignature = pPipelineStateRaw->GetEffect()->GetRootSignature();
        pCommandList->GetCommandList()->SetGraphicsRootSignature(rootSignature->GetRootSignature().get());
    }
}

void DrawingDevice_D3D12::SetDescriptorHeap(EDrawingDescriptorHeapType type, std::shared_ptr<ID3D12DescriptorHeap> pHeap)
{
    //if (m_pDescriptorHeaps[type] == pHeap)
    //    return;

    m_pDescriptorHeaps[type] = pHeap;

    uint32_t numDescriptorHeaps = 0;
    ID3D12DescriptorHeap* pDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};
    for (uint32_t i = 0; i < eDescriptorHeap_Count; ++i)
    {
        auto pDescriptorHeap = m_pDescriptorHeaps[i].get();
        if (pDescriptorHeap)
            pDescriptorHeaps[numDescriptorHeaps++] = pDescriptorHeap;
    }

    auto pCommandList = m_pDirectCommandManager->GetCommandList();
    pCommandList->GetCommandList()->SetDescriptorHeaps(numDescriptorHeaps, pDescriptorHeaps);
}

void DrawingDevice_D3D12::PushBlendState()
{
}

void DrawingDevice_D3D12::PopBlendState()
{
}

void DrawingDevice_D3D12::PushDepthState()
{
}

void DrawingDevice_D3D12::PopDepthState()
{
}

void DrawingDevice_D3D12::PushRasterState()
{
}

void DrawingDevice_D3D12::PopRasterState()
{
}

void DrawingDevice_D3D12::SetViewport(Box2* vp)
{
    auto pCommandList = m_pDirectCommandManager->GetCommandList();
    if (vp == nullptr)
        pCommandList->GetCommandList()->RSSetViewports(1, nullptr);
    else
    {
        const auto& topLeft = vp->mMin;
        const D3D12_VIEWPORT viewport{ topLeft.x, topLeft.y, vp->Width(), vp->Height(), 0, 1.f };
        pCommandList->GetCommandList()->RSSetViewports(1, &viewport);
    }

    auto m_ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
    pCommandList->GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);
}

void DrawingDevice_D3D12::SetTargets(std::shared_ptr<DrawingTarget> pTarget[], uint32_t maxTargets, std::shared_ptr<DrawingDepthBuffer> pDepthBuffer, std::shared_ptr<DrawingRWBuffer> pRWBuffer[], uint32_t maxRWBuffers)
{
    assert(maxTargets <= MAX_RENDER_TARGET_COUNT);

    auto pCommandList = m_pDirectCommandManager->GetCommandList();
    auto pDepthBufferRaw = pDepthBuffer != nullptr ? std::dynamic_pointer_cast<DrawingRawDepthTarget_D3D12>(pDepthBuffer->GetResource()) : nullptr;
    std::shared_ptr<DrawingRawFragmentTarget_D3D12> pTargetsRaw[MAX_RENDER_TARGET_COUNT] = { nullptr };

    if (maxTargets != 0)
    {
        std::transform(pTarget, pTarget + maxTargets, pTargetsRaw,
        [](std::shared_ptr<DrawingTarget> target) {
            return target != nullptr ? std::dynamic_pointer_cast<DrawingRawFragmentTarget_D3D12>(target->GetResource()) : nullptr;
        });
    }

    if (maxRWBuffers != 0)
    {
    }
    else
    {
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetDescriptors;
        for (uint32_t i = 0; i < maxTargets; ++i)
        {
            if (pTargetsRaw[i] != nullptr)
            {
                pCommandList->TransitionBarrier(pTargetsRaw[i]->GetTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET);

                auto renderTargetViewHandle = pTargetsRaw[i]->GetRenderTargetView();
                renderTargetDescriptors.push_back(renderTargetViewHandle);
            }
        }

        D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptors;
        if (pDepthBufferRaw != nullptr)
            depthStencilDescriptors = pDepthBufferRaw->GetDepthStencilView();

        pCommandList->GetCommandList()->OMSetRenderTargets(static_cast<UINT>(renderTargetDescriptors.size()), renderTargetDescriptors.data(), false, &depthStencilDescriptors);
    }
}

bool DrawingDevice_D3D12::UpdateEffectParameter(std::shared_ptr<DrawingParameter> pParam, std::shared_ptr<DrawingEffect> pEffect)
{
    assert(pEffect != nullptr);
    auto pRawEffect = std::dynamic_pointer_cast<DrawingRawEffect_D3D12>(pEffect->GetResource());
    assert(pRawEffect != nullptr);

    pRawEffect->UpdateParameter(pParam);

    return true;
}

bool DrawingDevice_D3D12::UpdateEffectTexture(std::shared_ptr<DrawingTexture> pTex, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    assert(pTex != nullptr);
    assert(pEffect != nullptr);

    auto pRawEffect = std::dynamic_pointer_cast<DrawingRawEffect_D3D12>(pEffect->GetResource());
    assert(pRawEffect != nullptr);

    auto pRawTex = std::dynamic_pointer_cast<DrawingRawTexture_D3D12>(pTex->GetResource());
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

bool DrawingDevice_D3D12::UpdateEffectBuffer(std::shared_ptr<DrawingTexBuffer> pBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

bool DrawingDevice_D3D12::UpdateEffectSampler(std::shared_ptr<DrawingSamplerState> pSampler, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

bool DrawingDevice_D3D12::UpdateEffectTexBuffer(std::shared_ptr<DrawingTexBuffer> pTexBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

bool DrawingDevice_D3D12::UpdateEffectRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

bool DrawingDevice_D3D12::UpdateEffectInputRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

bool DrawingDevice_D3D12::UpdateEffectOutputRWBuffer(std::shared_ptr<DrawingRWBuffer> pRWBuffer, std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    return true;
}

void DrawingDevice_D3D12::BeginEffect(DrawingContext& dc, std::shared_ptr<DrawingEffect> pEffect)
{
    assert(pEffect != nullptr);

    auto pRawEffect = std::dynamic_pointer_cast<DrawingRawEffect_D3D12>(pEffect->GetResource());
    assert(pRawEffect != nullptr);

    pRawEffect->Apply();
}

void DrawingDevice_D3D12::EndEffect(DrawingContext& dc, std::shared_ptr<DrawingEffect> pEffect)
{
    assert(pEffect != nullptr);

    auto pRawEffect = std::dynamic_pointer_cast<DrawingRawEffect_D3D12>(pEffect->GetResource());
    assert(pRawEffect != nullptr);

    pRawEffect->Terminate();
}

bool DrawingDevice_D3D12::DrawPrimitive(std::shared_ptr<DrawingPrimitive> pRes)
{
    assert(pRes != nullptr);

    auto pCommandList = m_pDirectCommandManager->GetCommandList();
    auto indexCount = pRes->GetIndexCount();
    auto instanceCount = pRes->GetInstanceCount();

    D3D12_PRIMITIVE_TOPOLOGY primType = D3D12Enum(pRes->GetPrimitiveType(), primType);
    pCommandList->GetCommandList()->IASetPrimitiveTopology(primType);

    pCommandList->FlushBarriers();

    // In D3D12, it's get rid of DrawIndexed and Draw function and instance must be over then 0 to draw something.
    if (instanceCount == 0)
        instanceCount = 1;

    if (indexCount != 0)
        pCommandList->GetCommandList()->DrawIndexedInstanced(indexCount, instanceCount, pRes->GetIndexOffset(), 0, pRes->GetInstanceOffset());
    else
        pCommandList->GetCommandList()->DrawInstanced(pRes->GetVertexCount(), pRes->GetInstanceCount(), pRes->GetVertexOffset(), pRes->GetInstanceOffset());

    return true;
}

bool DrawingDevice_D3D12::Present(const std::shared_ptr<DrawingTarget> pTarget, uint32_t syncInterval)
{
    auto pSwapChainRaw = std::static_pointer_cast<DrawingRawSwapChain_D3D12>(pTarget->GetResource());
    assert(pSwapChainRaw != nullptr);

    auto& backBuffer = pSwapChainRaw->GetTarget();
    auto pCommandList = m_pDirectCommandManager->GetCommandList();
    pCommandList->TransitionBarrier(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
    m_fenceValues[pSwapChainRaw->GetCurrentIndex()] = m_pDirectCommandManager->ExecuteAllCommandLists();

    HRESULT hr = pSwapChainRaw->Present(syncInterval);
    if (!SUCCEEDED(hr))
        return false;

    m_pDirectCommandManager->WaitForFenceValue(m_fenceValues[pSwapChainRaw->GetCurrentIndex()]);

    return true;
}

void* DrawingDevice_D3D12::Map(std::shared_ptr<DrawingResource> pRes, uint32_t subID, EDrawingAccessType flag, uint32_t& rowPitch, uint32_t& slicePitch, uint32_t offset, uint32_t sizeInBytes)
{
    assert(pRes != nullptr);

    if (subID < 0)
        subID = 0;

    switch(pRes->GetType())
    {
    case eResource_Texture:
        return MapResource<DrawingRawTexture_D3D12, DrawingTexture>(pRes, subID);

    case eResource_Vertex_Buffer:
        return MapResource<DrawingRawVertexBuffer_D3D12, DrawingVertexBuffer>(pRes, subID);

    case eResource_Index_Buffer:
        return MapResource<DrawingRawIndexBuffer_D3D12, DrawingIndexBuffer>(pRes, subID);
    }

    return nullptr;
}

void DrawingDevice_D3D12::UnMap(std::shared_ptr<DrawingResource> pRes, uint32_t subID)
{
    assert(pRes != nullptr);

    if (subID < 0)
        subID = 0;

    switch(pRes->GetType())
    {
    case eResource_Texture:
        return UnMapResource<DrawingRawTexture_D3D12, DrawingTexture>(pRes, subID);

    case eResource_Vertex_Buffer:
        return UnMapResource<DrawingRawVertexBuffer_D3D12, DrawingVertexBuffer>(pRes, subID);

    case eResource_Index_Buffer:
        return UnMapResource<DrawingRawIndexBuffer_D3D12, DrawingIndexBuffer>(pRes, subID);
    }
}

void DrawingDevice_D3D12::Flush()
{
    auto fenceValue = m_pDirectCommandManager->ExecuteAllCommandLists();
    m_pDirectCommandManager->WaitForFenceValue(fenceValue);

    fenceValue = m_pCopyCommandManager->ExecuteAllCommandLists();
    m_pCopyCommandManager->WaitForFenceValue(fenceValue);

    fenceValue = m_pComputeCommandManager->ExecuteAllCommandLists();
    m_pComputeCommandManager->WaitForFenceValue(fenceValue);
}

uint32_t DrawingDevice_D3D12::FormatBytes(EDrawingFormatType type)
{
    return D3D12FormatBytes(type);
}

std::shared_ptr<ID3D12Device2> DrawingDevice_D3D12::GetDevice() const
{
    return m_pDevice;
}

std::shared_ptr<IDXGIFactory4> DrawingDevice_D3D12::GetDXGIFactory() const
{
    return m_pDXGIFactory;
}

std::shared_ptr<DrawingCommandManager_D3D12> DrawingDevice_D3D12::GetCommandManager(EDrawingCommandListType type) const
{
    switch (type)
    {
        case eCommandList_Direct:
            return m_pDirectCommandManager;
        case eCommandList_Compute:
            return m_pComputeCommandManager;
        case eCommandList_Copy:
            return m_pCopyCommandManager;
        default:
            assert(false);
    }
    return nullptr;
}

bool DrawingDevice_D3D12::DoCreateEffect(const DrawingEffectDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingEffect>& pRes)
{
    return true;
}

bool DrawingDevice_D3D12::DoCreateVertexShader(const DrawingVertexShaderDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingVertexShader>& pRes)
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

bool DrawingDevice_D3D12::DoCreatePixelShader(const DrawingPixelShaderDesc& desc, const void* pData, uint32_t size, std::shared_ptr<DrawingPixelShader>& pRes)
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

std::shared_ptr<DrawingRawVertexShader_D3D12> DrawingDevice_D3D12::CreateVertexShaderFromBuffer(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const void* pData, uint32_t size)
{
    assert(pName != nullptr && pData != nullptr);
    auto pVertexShader = CreateVertexShaderFromBlob(pName, pData, size);

    if (pVertexShader != nullptr)
        return pVertexShader;

    assert(pEntryName != nullptr);
    return CreateVertexShaderFromString(pName, pEntryName, pSourceName, static_cast<const char*>(pData), size); 
}

std::shared_ptr<DrawingRawPixelShader_D3D12> DrawingDevice_D3D12::CreatePixelShaderFromBuffer(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const void* pData, uint32_t size)
{
    assert(pName != nullptr && pData != nullptr);
    auto pPixelShader = CreatePixelShaderFromBlob(pName, pData, size);

    if (pPixelShader != nullptr)
        return pPixelShader;

    assert(pEntryName != nullptr);
    return CreatePixelShaderFromString(pName, pEntryName, pSourceName, static_cast<const char*>(pData), size); 
}

std::shared_ptr<DrawingRawVertexShader_D3D12> DrawingDevice_D3D12::CreateVertexShaderFromBlob(std::shared_ptr<std::string> pName, const void* pShaderByteCode, uint32_t length)
{
    assert(pName != nullptr && pShaderByteCode != nullptr);

    ID3DBlob* pVSBlob;
    ID3D12ShaderReflection* pReflection;

    HRESULT hr = D3DCreateBlob(length, &pVSBlob);
    if (!SUCCEEDED(hr))
        return nullptr;
    memcpy(pVSBlob->GetBufferPointer(), pShaderByteCode, length);

    hr = D3DReflect(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&pReflection);
    if (!SUCCEEDED(hr))
        return nullptr;

    return std::make_shared<DrawingRawVertexShader_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), pName, std::shared_ptr<ID3D12ShaderReflection>(pReflection, D3D12Releaser<ID3D12ShaderReflection>), std::shared_ptr<ID3DBlob>(pVSBlob, D3D12Releaser<ID3DBlob>));
}

std::shared_ptr<DrawingRawPixelShader_D3D12> DrawingDevice_D3D12::CreatePixelShaderFromBlob(std::shared_ptr<std::string> pName, const void* pShaderByteCode, uint32_t length)
{
    assert(pName != nullptr && pShaderByteCode != nullptr);

    ID3DBlob* pPSBlob;
    ID3D12ShaderReflection* pReflection;

    HRESULT hr = D3DCreateBlob(length, &pPSBlob);
    if (!SUCCEEDED(hr))
        return nullptr;
    memcpy(pPSBlob->GetBufferPointer(), pShaderByteCode, length);

    hr = D3DReflect(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&pReflection);
    if (!SUCCEEDED(hr))
        return nullptr;

    return std::make_shared<DrawingRawPixelShader_D3D12>(std::static_pointer_cast<DrawingDevice_D3D12>(shared_from_this()), pName, std::shared_ptr<ID3D12ShaderReflection>(pReflection, D3D12Releaser<ID3D12ShaderReflection>), std::shared_ptr<ID3DBlob>(pPSBlob, D3D12Releaser<ID3DBlob>));
}

std::shared_ptr<DrawingRawVertexShader_D3D12> DrawingDevice_D3D12::CreateVertexShaderFromString(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const char* pSrc, uint32_t size)
{
    assert(pName != nullptr && pEntryName != nullptr && pSrc != nullptr);

    ID3DInclude* pInclude = (pSourceName == nullptr) ? nullptr : D3D_COMPILE_STANDARD_FILE_INCLUDE;
    DWORD flags = 0;
    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    HRESULT hr = D3DCompile(pSrc, size, pSourceName.get()->c_str(), nullptr, pInclude, pEntryName.get()->c_str(), "vs_5_1", flags, 0, &pShaderBlob, &pErrorBlob);
    if (!SUCCEEDED(hr))
    {
        auto err = pErrorBlob->GetBufferPointer();
        return false;
    }

    return CreateVertexShaderFromBlob(pName, pShaderBlob->GetBufferPointer(), (uint32_t)pShaderBlob->GetBufferSize());
}

std::shared_ptr<DrawingRawPixelShader_D3D12> DrawingDevice_D3D12::CreatePixelShaderFromString(std::shared_ptr<std::string> pName, std::shared_ptr<std::string> pEntryName, std::shared_ptr<std::string> pSourceName, const char* pSrc, uint32_t size)
{
    assert(pName != nullptr && pEntryName != nullptr && pSrc != nullptr);

    ID3DInclude* pInclude = (pSourceName == nullptr) ? nullptr : D3D_COMPILE_STANDARD_FILE_INCLUDE;
    DWORD flags = 0;
    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    HRESULT hr = D3DCompile(pSrc, size, pSourceName.get()->c_str(), nullptr, pInclude, pEntryName.get()->c_str(), "ps_5_1", flags, 0, &pShaderBlob, &pErrorBlob);
    if (!SUCCEEDED(hr))
    {
        auto err = pErrorBlob->GetBufferPointer();
        return false;
    }

    return CreatePixelShaderFromBlob(pName, pShaderBlob->GetBufferPointer(), (uint32_t)pShaderBlob->GetBufferSize());
}

template<typename T, typename U>
void* DrawingDevice_D3D12::MapResource(std::shared_ptr<DrawingResource> pRes, uint32_t subID)
{
    auto pResWrap = std::dynamic_pointer_cast<U>(pRes);
    assert(pResWrap != nullptr);

    auto pMapRes = std::dynamic_pointer_cast<T>(pResWrap->GetResource());
    assert(pMapRes != nullptr);

    void* pData = nullptr;
    auto pBuffer = pMapRes->GetBuffer();

    if (pBuffer != nullptr)
    {
        HRESULT hr = pBuffer->Map(subID, nullptr, &pData);
        assert(SUCCEEDED(hr));
    }

    return pData;
}

template<typename T, typename U>
void DrawingDevice_D3D12::UnMapResource(std::shared_ptr<DrawingResource> pRes, uint32_t aSubID)
{
    auto pResWrap = std::dynamic_pointer_cast<U>(pRes);
    assert(pResWrap != nullptr);

    auto pMapRes = std::dynamic_pointer_cast<T>(pResWrap->GetResource());
    assert(pMapRes != nullptr);

    auto pBuffer = pMapRes->GetBuffer();

    if (pBuffer != nullptr)
        pBuffer->Unmap(aSubID, nullptr);
}