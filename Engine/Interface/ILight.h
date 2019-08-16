#pragma once

namespace Engine
{
    enum ELightType
    {
        eLight_Directional = 0,
    };

    class ILight
    {
    public:
        ILight() {}
        virtual ~ILight() = default;

        virtual ELightType GetLightType() const = 0;
    };
}