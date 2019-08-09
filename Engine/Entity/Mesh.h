#pragma once

#include <memory>
#include <vector>

#include "IMesh.h"
#include "IRenderable.h"

namespace Engine
{
    class Mesh : public IMesh, public IRenderable
    {
    public:
        Mesh();
        virtual ~Mesh();

        void GetRenderable(RenderQueue &queue, const TransformComponent* pTransformComp) const override;

        const std::vector<std::shared_ptr<Attribute>> GetAttributes() const override;
        const std::shared_ptr<char> GetIndexData() const override;
        const uint32_t IndexSize() const override;

        const uint32_t VertexCount() const override;
        const uint32_t IndexCount() const override;

    protected:
        template<typename T>
        void AttachVertexData(const T array[], const uint32_t count, Attribute::ESemanticType type, Attribute::EFormatType format, std::string name);
        void AttachVertexData(const char array[], const uint32_t size, const uint32_t count, Attribute::ESemanticType type, Attribute::EFormatType format, std::string name);
        template<typename T>
        void AttachIndexData(const T array[], const uint32_t count);
        void AttachIndexData(const char array[], const uint32_t size, const uint32_t count);

    protected:
        std::vector<std::shared_ptr<Attribute>> m_pAttributes;
        std::shared_ptr<char> m_pIndexData;
        uint32_t m_indexSize;

        uint32_t m_vertexCount;
        uint32_t m_indexCount;
    };

    template<typename T>
    void Mesh::AttachVertexData(const T array[], const uint32_t count, Attribute::ESemanticType type, Attribute::EFormatType format, std::string name)
    {
        uint32_t size = count * sizeof(T);
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

    template<typename T>
    void Mesh::AttachIndexData(const T array[], const uint32_t count)
    {
        uint32_t size = count * sizeof(T);
        char* pData = new char[size];

        memcpy(pData, array, size);

        m_pIndexData = std::shared_ptr<char>(pData);
        m_indexSize = size;
        m_indexCount = count;
    }
}
