#pragma once

#include "ILight.h"

namespace Engine
{
    class DirectionalLight : public ILight
    {
    public:
        DirectionalLight();
        virtual ~DirectionalLight();

        ELightType GetLightType() const override;
    };
}