#include "Mesh.h"

using namespace Engine;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    m_pAttributes.clear();

    m_vertexCount = 0;
    m_indexCount = 0;
}

const std::vector<std::shared_ptr<Attribute>> Mesh::GetAttributes() const
{
    return m_pAttributes;
}

const std::shared_ptr<char> Mesh::GetIndexData() const
{
    return m_pIndexData;
}

const uint32_t Mesh::IndexSize() const
{
    return m_indexSize;
}

const uint32_t Mesh::VertexCount() const
{
    return m_vertexCount;
}

const uint32_t Mesh::IndexCount() const
{
    return m_indexCount;
}

void Mesh::AttachVertexData(const char array[], const uint32_t size, const uint32_t count, Attribute::ESemanticType type, Attribute::EFormatType format, std::string name)
{
    char* pData = new char[size];

    memcpy(pData, array, size);

    auto pAttribute = std::make_shared<Attribute>();
    pAttribute->semanticType = type;
    pAttribute->formatType = format;
    pAttribute->name = name;
    pAttribute->pData = std::shared_ptr<char>(pData);
    pAttribute->size = size;

    m_pAttributes.emplace_back(pAttribute);
    m_vertexCount = count;
}

void Mesh::AttachIndexData(const char array[], const uint32_t size, const uint32_t count)
{
    char* pData = new char[size];

    memcpy(pData, array, size);

    m_pIndexData = std::shared_ptr<char>(pData);
    m_indexSize = size;
    m_indexCount = count;
}