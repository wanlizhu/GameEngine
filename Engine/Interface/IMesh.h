#pragma once

#include <string>

#include "Vector.h"
#include "DrawingConstants.h"

namespace Engine
{
    struct Attribute
    {
        enum class ESemanticType : uint16_t
        {
            Position,
            Normal,
            Tangent,
            Texcoord0,
            Texcoord1,
            Texcoord2,
            Count,
        } semanticType;

        std::string name;
        std::shared_ptr<char> pData;
        uint32_t size;
    };

    class IMesh
    {
    public:
        IMesh() {}
        virtual ~IMesh() = default;

        virtual const std::vector<std::shared_ptr<Attribute>> GetAttributes() const = 0;
        virtual const std::shared_ptr<char> GetIndexData() const = 0;
        virtual const uint32_t IndexSize() const = 0;

        virtual const uint32_t VertexCount() const = 0;
        virtual const uint32_t IndexCount() const = 0;

        virtual void AttachVertexData(const char array[], const uint32_t size, const uint32_t count, Attribute::ESemanticType type, std::string name) = 0;
        virtual void AttachIndexData(const char array[], const uint32_t size, const uint32_t count) = 0;
    };
}