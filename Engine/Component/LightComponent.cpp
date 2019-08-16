#include "LightComponent.h"

using namespace Engine;

std::shared_ptr<ILight> LightComponent::GetLight() const
{
    return m_pLight;
}

void LightComponent::SetLight(std::shared_ptr<ILight> pLight)
{
    m_pLight = pLight;
}