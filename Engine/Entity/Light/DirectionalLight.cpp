#include "DirectionalLight.h"

using namespace Engine;

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::~DirectionalLight()
{
}

ELightType DirectionalLight::GetLightType() const
{
    return eLight_Directional;
}