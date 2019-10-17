#pragma once

#include <memory>

#include "ITexture.h"
#include "Vector.h"

namespace Engine
{
    class IMaterial
    {
    public:
        IMaterial() {}
        
        virtual ~IMaterial() = default;

        virtual std::shared_ptr<ITexture> GetAlbedoMap() const = 0;
        virtual void SetAlbedoMap(std::shared_ptr<ITexture> map) = 0;

        virtual float4 GetAlbedoColor() const = 0;
        virtual void SetAlbedoColor(float4& color) = 0;

        virtual std::shared_ptr<ITexture> GetMetallicRoughnessMap() const = 0;
        virtual void SetMetallicRoughnessMap(std::shared_ptr<ITexture> map) = 0;

        virtual float GetMetallic() const = 0;
        virtual void SetMetallic(float metallic) = 0;

        virtual float GetRoughness() const = 0;
        virtual void SetRoughness(float roughness) = 0;

        virtual std::shared_ptr<ITexture> GetNormalMap() const = 0;
        virtual void SetNormalMap(std::shared_ptr<ITexture> map) = 0;

        virtual std::shared_ptr<ITexture> GetHeightMap() const = 0;
        virtual void SetHeightMap(std::shared_ptr<ITexture> map) = 0;

        virtual std::shared_ptr<ITexture> GetOcclusionMap() const = 0;
        virtual void SetOcclusionMap(std::shared_ptr<ITexture> map) = 0;

        virtual std::shared_ptr<ITexture> GetEmissiveMap() const = 0;
        virtual void SetEmissiveMap(std::shared_ptr<ITexture> map) = 0;

        virtual float3 GetEmissive() const = 0;
        virtual void SetEmissive(float* emissive) = 0;
    };
}