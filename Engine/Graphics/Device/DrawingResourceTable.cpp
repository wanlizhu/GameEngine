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

bool DrawingResourceFactory::CreateResource(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, DrawingResourceTable& resTable, const void* pData, uint32_t size) const
{
    bool result = false;

    switch (pDesc->GetType())
    {
    case eResource_Effect:          result = CreateEffect(pDesc, pRes); break;
    case eResource_Vertex_Format:   result = CreateVertexFormat(pDesc, pRes); break;
    case eResource_Vertex_Buffer:   result = CreateVertexBuffer(pDesc, pRes, pData, size); break;
    case eResource_Index_Buffer:    result = CreateIndexBuffer(pDesc, pRes, pData, size); break;
    case eResource_Texture:         result = CreateTexture(pDesc, pRes, pData, size); break;
    case eResource_Blend_State:     result = CreateBlendState(pDesc, pRes); break;
    case eResource_Depth_State:     result = CreateDepthState(pDesc, pRes); break;
    case eResource_Raster_State:    result = CreateRasterState(pDesc, pRes); break;
    case eResource_Sampler_State:   result = CreateSamplerState(pDesc, pRes); break;
    case eResource_Vertex_Shader:   result = CreateVertexShader(pDesc, pRes); break;
    case eResource_Pixel_Shader:    result = CreatePixelShader(pDesc, pRes); break;
    case eResource_Primitive:       result = CreatePrimitive(pDesc, pRes); break;
    case eResource_Varing_States:   result = CreateVaringStates(pDesc, pRes); break;
    case eResource_Pipeline_State:  result = CreatePipelineState(pDesc, pRes, resTable); break;
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

bool DrawingResourceFactory::CreateTexture(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, const void* pData, uint32_t size) const
{
    auto pTextureDesc = std::static_pointer_cast<const DrawingTextureDesc>(pDesc);
    if (pTextureDesc == nullptr)
        return false;

    std::shared_ptr<DrawingTexture> pTexture;
    bool result = m_pDevice->CreateTexture(*pTextureDesc, pTexture, pData, size);
    pRes = pTexture;

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

bool DrawingResourceFactory::CreatePipelineState(const std::shared_ptr<DrawingResourceDesc>& pDesc, std::shared_ptr<DrawingResource>& pRes, DrawingResourceTable& resTable) const
{
    auto pPipelineStateDesc = std::static_pointer_cast<const DrawingPipelineStateDesc>(pDesc);
    if (pPipelineStateDesc == nullptr)
        return false;

    std::shared_ptr<DrawingPipelineState> pPipelineState;

    DrawingPipelineState::SubobjectResourceTable subobjectResources;
    for (const auto& subobject : pPipelineStateDesc->mSubobjectTable)
        subobjectResources.emplace(subobject.first, resTable.GetResourceEntry(subobject.second)->GetResource());

    bool result = m_pDevice->CreatePipelineState(*pPipelineStateDesc, subobjectResources, pPipelineState);
    pRes = pPipelineState;

    return true;
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

    if (!m_factory.CreateResource(m_pDesc, pRes, m_resTable, m_pData, m_size))
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

void DrawingResourceTable::ResourceEntry::SetInitData(const void* pData, uint32_t size)
{
    m_pData = pData;
    m_size = size;
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
    m_pDesc(pDesc), m_pRes(nullptr), m_pData(nullptr), m_size(0), m_factory(factory), m_resTable(table)
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