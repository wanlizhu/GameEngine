#pragma once

#include "Component.h"
#include "ILight.h"

namespace Engine
{
    class LightComponent : public ComponentBase<LightComponent>
    {
    public:
        LightComponent() : ComponentBase<LightComponent>() {}
        virtual ~LightComponent() = default;

        std::shared_ptr<ILight> GetLight() const;
        void SetLight(std::shared_ptr<ILight> pLight);

    private:
        std::shared_ptr<ILight> m_pLight = nullptr;
    };
}