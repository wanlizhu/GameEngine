#include "DrawingDevice.h"
#include "DrawingResourceDesc.h"

#include "DrawingEffectPool.h"

using namespace Engine;

DrawingEffectPool::DrawingEffectPool(const std::shared_ptr<DrawingDevice> pDevice) : m_pDevice(pDevice)
{
}

DrawingEffectPool::~DrawingEffectPool()
{
    ClearEffectTable();
    ClearVertexShaderTable();
    ClearPixelShaderTable();

    m_pDevice = nullptr;
}

bool DrawingEffectPool::LoadEffectFromBuffer(const DrawingEffectDesc& desc, std::shared_ptr<DrawingResource>& pRes)
{
    std::shared_ptr<DrawingEffect> pEffect = nullptr;

    bool result = LoadFromBuffer(m_effectTable, desc, pEffect);
    pRes = pEffect;
    return result;
}

bool DrawingEffectPool::LoadEffectFromString(const DrawingEffectDesc& desc, std::shared_ptr<DrawingResource>& pRes)
{
    std::shared_ptr<DrawingEffect> pEffect = nullptr;

    bool result = LoadFromString(m_effectTable, desc, pEffect);
    pRes = pEffect;
    return result;
}

bool DrawingEffectPool::LoadEffectFromFile(const DrawingEffectDesc& desc, std::shared_ptr<DrawingResource>& pRes)
{
    std::shared_ptr<DrawingEffect> pEffect = nullptr;

    bool result = LoadFromFile(m_effectTable, desc, pEffect);
    pRes = pEffect;
    return result;
}

bool DrawingEffectPool::LoadEffectFromShader(const DrawingEffectDesc& desc, std::shared_ptr<DrawingResource>& pRes)
{
    auto pVSName = desc.GetResourceDescName(DrawingLinkedEffectDesc::VERTEX_SHADER_ID);
    auto pPSName = desc.GetResourceDescName(DrawingLinkedEffectDesc::PIXEL_SHADER_ID);

    if ((pVSName == nullptr) || (pPSName == nullptr))
        return false;

    std::shared_ptr<DrawingVertexShader> pVSShader;
    std::shared_ptr<DrawingPixelShader> pPSShader;

    if (!(pVSShader = GetVertexShader(pVSName)))
        return false;

    if (!(pPSShader = GetPixelShader(pPSName)))
        return false;

    std::shared_ptr<DrawingEffect> pEffect = nullptr;

    if (!m_pDevice->CreateEffectFromShader(desc, pVSShader, pPSShader, pEffect))
        return false;

    pRes = pEffect;
    return true;
}

bool DrawingEffectPool::LoadVertexShaderFromBuffer(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    bool result = LoadFromBuffer(m_vertexShaderTable, desc, pRes);
    return result;
}

bool DrawingEffectPool::LoadVertexShaderFromString(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    bool result = LoadFromString(m_vertexShaderTable, desc, pRes);
    return result;
}

bool DrawingEffectPool::LoadVertexShaderFromFile(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    bool result = LoadFromFile(m_vertexShaderTable, desc, pRes);
    return result;
}

bool DrawingEffectPool::LoadPixelShaderFromBuffer(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    bool result = LoadFromBuffer(m_pixelShaderTable, desc, pRes);
    return result;
}

bool DrawingEffectPool::LoadPixelShaderFromString(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    bool result = LoadFromString(m_pixelShaderTable, desc, pRes);
    return result;
}

bool DrawingEffectPool::LoadPixelShaderFromFile(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    bool result = LoadFromFile(m_pixelShaderTable, desc, pRes);
    return result;
}

std::shared_ptr<DrawingEffect> DrawingEffectPool::GetEffect(std::shared_ptr<std::string> pName)
{
    auto it = m_effectTable.find(pName);
    if (it == m_effectTable.cend())
        return nullptr;

    return it->second;
}

bool DrawingEffectPool::AddEffectToPool(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect)
{
    auto it = m_effectTable.find(pName);
    if (it != m_effectTable.cend())
        return false;

    m_effectTable.emplace(pName, pEffect);
    return true;
}

bool DrawingEffectPool::RemoveEffectFromPool(std::shared_ptr<std::string> pName)
{
    auto it = m_effectTable.find(pName);
    if (it == m_effectTable.cend())
        return false;

    auto pRes = it->second;
    if (pRes != nullptr)
        pRes = nullptr;

    m_effectTable.erase(it);
    return true;
}

std::shared_ptr<DrawingVertexShader> DrawingEffectPool::GetVertexShader(std::shared_ptr<std::string> pName)
{
    auto it = m_vertexShaderTable.find(pName);
    if (it == m_vertexShaderTable.cend())
        return nullptr;

    return it->second;
}

bool DrawingEffectPool::AddVertexShaderToPool(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingVertexShader> pVertexShader)
{
    auto it = m_vertexShaderTable.find(pName);
    if (it != m_vertexShaderTable.cend())
        return false;

    m_vertexShaderTable.emplace(pName, pVertexShader);
    return true;
}

bool DrawingEffectPool::RemoveVertexShaderFromPool(std::shared_ptr<std::string> pName)
{
    auto it = m_vertexShaderTable.find(pName);
    if (it == m_vertexShaderTable.cend())
        return false;

    auto pRes = it->second;
    if (pRes != nullptr)
        pRes = nullptr;

    m_vertexShaderTable.erase(it);
    return true;
}

std::shared_ptr<DrawingPixelShader> DrawingEffectPool::GetPixelShader(std::shared_ptr<std::string> pName)
{
    auto it = m_pixelShaderTable.find(pName);
    if (it == m_pixelShaderTable.cend())
        return nullptr;

    return it->second;
}

bool DrawingEffectPool::AddPixelShaderToPool(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingPixelShader> pPixelShader)
{
    auto it = m_pixelShaderTable.find(pName);
    if (it != m_pixelShaderTable.cend())
        return false;

    m_pixelShaderTable.emplace(pName, pPixelShader);
    return true;
}

bool DrawingEffectPool::RemovePixelShaderFromPool(std::shared_ptr<std::string> pName)
{
    auto it = m_pixelShaderTable.find(pName);
    if (it == m_pixelShaderTable.cend())
        return false;

    auto pRes = it->second;
    if (pRes != nullptr)
        pRes = nullptr;

    m_pixelShaderTable.erase(it);
    return true;
}

void DrawingEffectPool::ClearEffectTable()
{
    std::for_each(m_effectTable.begin(), m_effectTable.end(), [](EffectTableType::value_type& aElem)
    {
        auto& effect = aElem.second;
        if (effect != nullptr)
            effect.reset();
    });

    m_effectTable.clear();
}

void DrawingEffectPool::ClearVertexShaderTable()
{
    std::for_each(m_vertexShaderTable.begin(), m_vertexShaderTable.end(), [](VertexShaderTableType::value_type& aElem)
    {
        auto& shader = aElem.second;
        if (shader != nullptr)
            shader.reset();
    });

    m_vertexShaderTable.clear();
}

void DrawingEffectPool::ClearPixelShaderTable()
{
    std::for_each(m_pixelShaderTable.begin(), m_pixelShaderTable.end(), [](PixelShaderTableType::value_type& aElem)
    {
        auto& shader = aElem.second;
        if (shader != nullptr)
            shader.reset();
    });

    m_pixelShaderTable.clear();
}

template<typename TypeN>
bool DrawingEffectPool::Load(std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<TypeN>>& table, std::shared_ptr<std::string> pName, std::shared_ptr<TypeN>& pRes)
{
    auto it = table.find(pName);
    if (it == table.cend())
        return false;

    pRes = it->second;
    return true;
}

template<typename TypeN, typename DescN>
bool DrawingEffectPool::LoadFromBuffer(std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<TypeN>>& table, const DescN& desc, std::shared_ptr<TypeN>& pRes)
{
    if ((desc.mProgramType != eProgram_Binary) &&
        (desc.mProgramType != eProgram_String))
        return false;

    if (Load(table, desc.mpName, pRes))
    {
        if (pRes != nullptr)
            return true;
        
        return false;
    }

    void* pData = desc.mpSourceName->data();
    uint32_t length = (uint32_t)desc.mpSourceName->length();

    if (!DoCreateFromBuffer(pData, length, desc, pRes))
        return false;

    table.emplace(desc.mpName, pRes);
    return true;
}

template<typename TypeN, typename DescN>
bool DrawingEffectPool::LoadFromString(std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<TypeN>>& table, const DescN& desc, std::shared_ptr<TypeN>& pRes)
{
    if (desc.mProgramType != eProgram_String)
        return false;

    if (Load(table, desc.mpName, pRes))
    {
        if (pRes != nullptr)
            return true;
        
        return false;
    }

    if (!DoCreateFromString(*(desc.mpSourceName), desc, pRes))
        return false;

    table.emplace(desc.mpName, pRes);
    return true;
}

template<typename TypeN, typename DescN>
bool DrawingEffectPool::LoadFromFile(std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<TypeN>>& table, const DescN& desc, std::shared_ptr<TypeN>& pRes)
{
    if (desc.mProgramType != eProgram_File)
        return false;

    if (Load(table, desc.mpName, pRes))
    {
        if (pRes != nullptr)
            return true;
        
        return false;
    }

    if (!DoCreateFromFile(desc, pRes))
        return false;

    table.emplace(desc.mpName, pRes);
    return true;
}

template<>
bool DrawingEffectPool::DoCreateFromBuffer<DrawingEffect, DrawingEffectDesc>(const void* pData, uint32_t length, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes)
{
    return true;
}

template<>
bool DrawingEffectPool::DoCreateFromBuffer<DrawingVertexShader, DrawingVertexShaderDesc>(const void* pData, uint32_t length, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    return m_pDevice->CreateVertexShaderFromBuffer(pData, length, desc, pRes);
}

template<>
bool DrawingEffectPool::DoCreateFromBuffer<DrawingPixelShader, DrawingPixelShaderDesc>(const void* pData, uint32_t length, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    return m_pDevice->CreatePixelShaderFromBuffer(pData, length, desc, pRes);
}

template<>
bool DrawingEffectPool::DoCreateFromString<DrawingEffect, DrawingEffectDesc>(std::string& str, const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes)
{
    return true;
}

template<>
bool DrawingEffectPool::DoCreateFromString<DrawingVertexShader, DrawingVertexShaderDesc>(std::string& str, const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    return m_pDevice->CreateVertexShaderFromString(str, desc, pRes);
}

template<>
bool DrawingEffectPool::DoCreateFromString<DrawingPixelShader, DrawingPixelShaderDesc>(std::string& str, const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    return m_pDevice->CreatePixelShaderFromString(str, desc, pRes);
}

template<>
bool DrawingEffectPool::DoCreateFromFile<DrawingEffect, DrawingEffectDesc>(const DrawingEffectDesc& desc, std::shared_ptr<DrawingEffect>& pRes)
{
    return true;
}

template<>
bool DrawingEffectPool::DoCreateFromFile<DrawingVertexShader, DrawingVertexShaderDesc>(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes)
{
    return m_pDevice->CreateVertexShaderFromFile(desc, pRes);
}

template<>
bool DrawingEffectPool::DoCreateFromFile<DrawingPixelShader, DrawingPixelShaderDesc>(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes)
{
    return m_pDevice->CreatePixelShaderFromFile(desc, pRes);
}