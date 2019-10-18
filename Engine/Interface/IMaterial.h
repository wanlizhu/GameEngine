#pragma once

#include <memory>

#include "ITexture.h"
#include "Vector.h"

namespace Engine
{
    enum EMaterialType
    {
        eMaterial_Standard = 0,
    };

    class IMaterial
    {
    public:
        IMaterial() {}
        virtual ~IMaterial() = default;

        virtual EMaterialType GetMaterialType() const = 0;
    };
}