#include "DrawingDevice.h"
#include "DrawingResourceDesc.h"
#include "DrawingEffectPool.h"

#include "DrawingResourceTable.h"

using namespace Engine;

DrawingResourceFactory::DrawingResourceFactory(const std::shared_ptr<DrawingDevice> pDevice) : m_pDevice(pDevice)
{
}

DrawingResourceFactory::~DrawingResourceFactory()
{
    m_pDevice = nullptr;
}

void DrawingResourceFactory::SetEffectPool(const std::weak_ptr<DrawingEffectPool> pEffectPool)
{
    m_pEffectPool = pEffectPool;
}

bool DrawingResourceFactory::CreateResource(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, DrawingResourceTable& resTable, const void* pData[], uint32_t size[], uint32_t slices) const
{
    bool result = false;

    const void* pInitData = (slices == 0) ? nullptr : pData[0];
    uint32_t initDataSize = (slices == 0) ? 0 : size[0];

    switch (pDesc->GetType())
    {
    case eResource_Effect:          result = CreateEffect(pDesc, pRes); break;
    case eResource_Vertex_Format:   result = CreateVertexFormat(pDesc, pRes); break;
    case eResource_Vertex_Buffer:   result = CreateVertexBuffer(pDesc, pRes, pInitData, initDataSize); break;
    case eResource_Index_Buffer:    result = CreateIndexBuffer(pDesc, pRes, pInitData, initDataSize); break;
    case eResource_Constant_Buffer: result = CreateConstantBuffer(pDesc, pRes); break;
    case eResource_Texture:         result = CreateTexture(pDesc, pRes, pData, size, slices); break;
    case eResource_Target:          result = CreateTarget(pDesc, pRes); break;
    case eResource_DepthBuffer:     result = CreateDepthBuffer(pDesc, pRes); break;
    case eResource_Blend_State:     result = CreateBlendState(pDesc, pRes); break;
    case eResource_Depth_State:     result = CreateDepthState(pDesc, pRes); break;
    case eResource_Raster_State:    result = CreateRasterState(pDesc, pRes); break;
    case eResource_Sampler_State:   result = CreateSamplerState(pDesc, pRes); break;
    case eResource_Vertex_Shader:   result = CreateVertexShader(pDesc, pRes); break;
    case eResource_Pixel_Shader:    result = CreatePixelShader(pDesc, pRes); break;
    case eResource_Primitive:       result = CreatePrimitive(pDesc, pRes); break;
    case eResource_Varing_States:   result = CreateVaringStates(pDesc, pRes); break;
    default:
        result = false;
        break;
    }

    return result;
}

bool DrawingResourceFactory::CreateEffect(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pEffectDesc = std::static_pointer_cast<const DrawingEffectDesc>(pDesc);
    if (pEffectDesc == nullptr)
        return false;

    auto pEffectPool = m_pEffectPool.lock();
    if (pEffectPool == nullptr)
        return false;

    auto pEffect = pEffectPool->GetEffect(pEffectDesc->mpName);
    if (pEffect != nullptr)
    {
        pRes = pEffect;
        return true;
    }

    switch (pEffectDesc->mProgramType)
    {
        case eProgram_Shader:
            return pEffectPool->LoadEffectFromShader(*pEffectDesc, pRes);
        case eProgram_Binary:
            return pEffectPool->LoadEffectFromBuffer(*pEffectDesc, pRes);
        case eProgram_String:
            return pEffectPool->LoadEffectFromString(*pEffectDesc, pRes);
        case eProgram_File:
            return pEffectPool->LoadEffectFromFile(*pEffectDesc, pRes);
        default:
            return false;
    }
    return true;
}

bool DrawingResourceFactory::CreateVertexFormat(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pVertexFormatDesc = std::static_pointer_cast<const DrawingVertexFormatDesc>(pDesc);
    if (pVertexFormatDesc == nullptr)
        return false;

    std::shared_ptr<DrawingVertexFormat> pVertexFormat;
    bool result = m_pDevice->CreateVertexFormat(*pVertexFormatDesc, pVertexFormat);
    pRes = pVertexFormat;

    return result;
}

bool DrawingResourceFactory::CreateVertexBuffer(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, const void* pData, uint32_t size) const
{
    auto pVertexBufferDesc = std::static_pointer_cast<const DrawingVertexBufferDesc>(pDesc);
    if (pVertexBufferDesc == nullptr)
        return false;

    std::shared_ptr<DrawingVertexBuffer> pVertexBuffer;
    bool result = m_pDevice->CreateVertexBuffer(*pVertexBufferDesc, pVertexBuffer, nullptr, pData, size);
    pRes = pVertexBuffer;

    return result;
}

bool DrawingResourceFactory::CreateIndexBuffer(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, const void* pData, uint32_t size) const
{
    auto pIndexBufferDesc = std::static_pointer_cast<const DrawingIndexBufferDesc>(pDesc);
    if (pIndexBufferDesc == nullptr)
        return false;

    std::shared_ptr<DrawingIndexBuffer> pIndexBuffer;
    bool result = m_pDevice->CreateIndexBuffer(*pIndexBufferDesc, pIndexBuffer, nullptr, pData, size);
    pRes = pIndexBuffer;

    return result;
}

bool DrawingResourceFactory::CreateTexture(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, const void* pData[], uint32_t size[], uint32_t slices) const
{
    auto pTextureDesc = std::static_pointer_cast<const DrawingTextureDesc>(pDesc);
    if (pTextureDesc == nullptr)
        return false;

    std::shared_ptr<DrawingTexture> pTexture;
    bool result = m_pDevice->CreateTexture(*pTextureDesc, pTexture, nullptr, pData, size, slices);
    pRes = pTexture;

    return result;
}

bool DrawingResourceFactory::CreateTarget(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pTargetDesc = std::static_pointer_cast<const DrawingTargetDesc>(pDesc);
    if (pTargetDesc == nullptr)
        return false;

    std::shared_ptr<DrawingTarget> pTarget;
    bool result = m_pDevice->CreateTarget(*pTargetDesc, pTarget);
    pRes = pTarget;

    return result;
}

bool DrawingResourceFactory::CreateDepthBuffer(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pDepthBufferDesc = std::static_pointer_cast<const DrawingDepthBufferDesc>(pDesc);
    if (pDepthBufferDesc == nullptr)
        return false;

    std::shared_ptr<DrawingDepthBuffer> pDepthBuffer;
    bool result = m_pDevice->CreateDepthBuffer(*pDepthBufferDesc, pDepthBuffer);
    pRes = pDepthBuffer;

    return result;
}

bool DrawingResourceFactory::CreateConstantBuffer(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pConstantBufferDesc = std::static_pointer_cast<const DrawingConstantBufferDesc>(pDesc);
    if (pConstantBufferDesc == nullptr)
        return false;

    std::shared_ptr<DrawingConstantBuffer> pConstantBuffer;
    bool result = m_pDevice->CreateConstantBuffer(*pConstantBufferDesc, pConstantBuffer);
    pRes = pConstantBuffer;

    return result;
}

bool DrawingResourceFactory::CreateBlendState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pBlendStateDesc = std::static_pointer_cast<const DrawingBlendStateDesc>(pDesc);
    if (pBlendStateDesc == nullptr)
        return false;

    std::shared_ptr<DrawingBlendState> pBlendState;
    bool result = m_pDevice->CreateBlendState(*pBlendStateDesc, pBlendState);
    pRes = pBlendState;

    return result;
}

bool DrawingResourceFactory::CreateDepthState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pDepthStateDesc = std::static_pointer_cast<const DrawingDepthStateDesc>(pDesc);
    if (pDepthStateDesc == nullptr)
        return false;

    std::shared_ptr<DrawingDepthState> pDepthState;
    bool result = m_pDevice->CreateDepthState(*pDepthStateDesc, pDepthState);
    pRes = pDepthState;

    return result;
}

bool DrawingResourceFactory::CreateRasterState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pRasterStateDesc = std::static_pointer_cast<const DrawingRasterStateDesc>(pDesc);
    if (pRasterStateDesc == nullptr)
        return false;

    std::shared_ptr<DrawingRasterState> pRasterState;
    bool result = m_pDevice->CreateRasterState(*pRasterStateDesc, pRasterState);
    pRes = pRasterState;

    return result;
}

bool DrawingResourceFactory::CreateSamplerState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pSamplerStateDesc = std::static_pointer_cast<const DrawingSamplerStateDesc>(pDesc);
    if (pSamplerStateDesc == nullptr)
        return false;

    std::shared_ptr<DrawingSamplerState> pSamplerState;
    bool result = m_pDevice->CreateSamplerState(*pSamplerStateDesc, pSamplerState);
    pRes = pSamplerState;

    return result;
}

bool DrawingResourceFactory::CreateVertexShader(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pEffectPool = m_pEffectPool.lock();
    auto pVertexShaderDesc = std::static_pointer_cast<const DrawingVertexShaderDesc>(pDesc);
    if (pVertexShaderDesc == nullptr)
        return false;

    std::shared_ptr<DrawingVertexShader> pVertexShader = pEffectPool->GetVertexShader(pVertexShaderDesc->mpName);
    if (pVertexShader != nullptr)
    {
        pRes = pVertexShader;
        return true;
    }

    bool result = false;
    switch (pVertexShaderDesc->mProgramType)
    {
        case eProgram_Binary:
        {
            result = pEffectPool->LoadVertexShaderFromBuffer(*pVertexShaderDesc, pVertexShader);
            break;
        }
        case eProgram_String:
        {
            result = pEffectPool->LoadVertexShaderFromString(*pVertexShaderDesc, pVertexShader);
            break;
        }
        case eProgram_File:
        {
            result = pEffectPool->LoadVertexShaderFromFile(*pVertexShaderDesc, pVertexShader);
            break;
        }
        default:
            return false;
    }

    pRes = pVertexShader;
    return true;
}

bool DrawingResourceFactory::CreatePixelShader(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pEffectPool = m_pEffectPool.lock();
    auto pPixelShaderDesc = std::static_pointer_cast<const DrawingPixelShaderDesc>(pDesc);
    if (pPixelShaderDesc == nullptr)
        return false;

    std::shared_ptr<DrawingPixelShader> pPixelShader = pEffectPool->GetPixelShader(pPixelShaderDesc->mpName);
    if (pPixelShader != nullptr)
    {
        pRes = pPixelShader;
        return true;
    }

    bool result = false;
    switch (pPixelShaderDesc->mProgramType)
    {
        case eProgram_Binary:
        {
            result = pEffectPool->LoadPixelShaderFromBuffer(*pPixelShaderDesc, pPixelShader);
            break;
        }
        case eProgram_String:
        {
            result = pEffectPool->LoadPixelShaderFromString(*pPixelShaderDesc, pPixelShader);
            break;
        }
        case eProgram_File:
        {
            result = pEffectPool->LoadPixelShaderFromFile(*pPixelShaderDesc, pPixelShader);
            break;
        }
        default:
            return false;
    }

    pRes = pPixelShader;
    return true;
}

bool DrawingResourceFactory::CreatePrimitive(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pPrimitiveDesc = std::static_pointer_cast<const DrawingPrimitiveDesc>(pDesc);
    if (pPrimitiveDesc == nullptr)
        return false;

    std::shared_ptr<DrawingPrimitive> pPrimitive;
    bool result = m_pDevice->CreatePrimitive(*pPrimitiveDesc, pPrimitive);
    pRes = pPrimitive;

    return result;
}

bool DrawingResourceFactory::CreateVaringStates(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes) const
{
    auto pVaringStatesDesc = std::static_pointer_cast<const DrawingVaringStatesDesc>(pDesc);
    if (pVaringStatesDesc == nullptr)
        return false;

    std::shared_ptr<DrawingVaringStates> pVaringStates;
    bool result = m_pDevice->CreateVaringStates(*pVaringStatesDesc, pVaringStates);
    pRes = pVaringStates;

    return result;
}

DrawingResourceTable::ResourceEntry::~ResourceEntry()
{
    m_pDesc = nullptr;
    m_pRes = nullptr;
}

bool DrawingResourceTable::ResourceEntry::CreateResource()
{
    if (m_pDesc->IsExternalResource())
        return true;

    if (m_pRes != nullptr)
        return true;

    std::shared_ptr<DrawingResource> pRes = nullptr;

    LoadPrecedingResources();

    if (!m_factory.CreateResource(m_pDesc, pRes, m_resTable, m_pData, m_size, m_slices))
        return false;

    m_pRes = pRes;

    return true;
}

std::shared_ptr<DrawingResource> DrawingResourceTable::ResourceEntry::GetResource()
{
    return m_pRes;
}

const std::shared_ptr<DrawingResourceDesc> DrawingResourceTable::ResourceEntry::GetDesc() const
{
    return m_pDesc;
}

void DrawingResourceTable::ResourceEntry::SetDesc(std::shared_ptr<DrawingResourceDesc> pDesc)
{
    if (m_pDesc != pDesc)
    {
        m_pDesc = pDesc;
        m_pRes = nullptr;
    }
}

void DrawingResourceTable::ResourceEntry::SetInitData(uint32_t index, const void* pData, uint32_t size)
{
    m_pData[index] = pData;
    m_size[index] = size;
}

void DrawingResourceTable::ResourceEntry::SetInitDataSlices(uint32_t slices)
{
    m_slices = slices;
}

bool DrawingResourceTable::ResourceEntry::SetExternalResource(std::shared_ptr<DrawingResource> pRes)
{
    assert(m_pDesc != nullptr);

    if (!m_pDesc->IsExternalResource())
        return false;

    if (pRes != nullptr)
        if (m_pDesc->GetType() != pRes->GetType())
            return false;

    m_pRes = pRes;
    return true;
}

DrawingResourceTable::ResourceEntry::ResourceEntry(std::shared_ptr<DrawingResourceDesc> pDesc, const DrawingResourceFactory& factory, DrawingResourceTable& table) :
    m_pDesc(pDesc), m_pRes(nullptr), m_factory(factory), m_resTable(table)
{
}

void DrawingResourceTable::ResourceEntry::LoadPrecedingResources()
{
    auto resources = m_pDesc->GetResourceDescNames();
    std::for_each(resources.cbegin(), resources.cend(), [&](DrawingResourceDesc::ResourceDescNamesType::value_type aElem)
    {
        if (auto name = aElem.second)
        {
            auto entry = m_resTable.GetResourceEntry(name);
            if (entry != nullptr)
                entry->CreateResource();
        }
    });
}

DrawingResourceTable::DrawingResourceTable(const DrawingResourceFactory& factory) : m_factory(factory)
{
}

DrawingResourceTable::~DrawingResourceTable()
{
    ClearResourceEntries();
}

std::shared_ptr<DrawingResourceTable::ResourceEntry> DrawingResourceTable::GetResourceEntry(std::shared_ptr<std::string> pName) const
{
    auto it = m_resourceTable.find(pName);
    if (it == m_resourceTable.cend())
        return nullptr;

    return it->second;
}

bool DrawingResourceTable::AddResourceEntry(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingResourceDesc> pDesc)
{
    auto it = m_resourceTable.find(pName);
    if (it != m_resourceTable.cend())
        return false;

    std::shared_ptr<ResourceEntry> pEntry(new ResourceEntry(pDesc, m_factory, *this));
    m_resourceTable.emplace(pName, pEntry);
    return true;
}

bool DrawingResourceTable::RemoveResourceEntry(std::shared_ptr<std::string> pName)
{
    auto it = m_resourceTable.find(pName);
    if (it == m_resourceTable.cend())
        return false;

    m_resourceTable.erase(it);
    return true;
}

void DrawingResourceTable::ClearResourceEntries()
{
    m_resourceTable.clear();
}

bool DrawingResourceTable::BuildResources()
{
    bool result = true;
    std::for_each(m_resourceTable.begin(), m_resourceTable.end(), [&result](std::pair<std::shared_ptr<std::string>, std::shared_ptr<ResourceEntry>> elem){
        if (auto& entry = elem.second)
        {
            if (!entry->CreateResource())
                result = false;
        }
    });

    return result;
}