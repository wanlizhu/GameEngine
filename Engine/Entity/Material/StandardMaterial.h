#pragma once

#include "Vector.h"
#include "IMaterial.h"

namespace Engine
{
    class StandardMaterial : public IMaterial
    {
    public:
        StandardMaterial();
        virtual ~StandardMaterial();

        EMaterialType GetMaterialType() const override
        {
            return eMaterial_Standard;
        }

        std::shared_ptr<ITexture> GetAlbedoMap() const;
        void SetAlbedoMap(std::shared_ptr<ITexture> map);

        float4 GetAlbedoColor() const;
        void SetAlbedoColor(float4& color);

        std::shared_ptr<ITexture> GetMetallicRoughnessMap() const;
        void SetMetallicRoughnessMap(std::shared_ptr<ITexture> map);

        float GetMetallic() const;
        void SetMetallic(float metallic);

        float GetRoughness() const;
        void SetRoughness(float roughness);

        std::shared_ptr<ITexture> GetNormalMap() const;
        void SetNormalMap(std::shared_ptr<ITexture> map);

        std::shared_ptr<ITexture> GetHeightMap() const;
        void SetHeightMap(std::shared_ptr<ITexture> map);

        std::shared_ptr<ITexture> GetOcclusionMap() const;
        void SetOcclusionMap(std::shared_ptr<ITexture> map);

        std::shared_ptr<ITexture> GetEmissiveMap() const;
        void SetEmissiveMap(std::shared_ptr<ITexture> map);

        float3 GetEmissive() const;
        void SetEmissive(float* emissive);

    protected:
        std::shared_ptr<ITexture> m_pAlbedoMap;
        float4 m_albedoColor;

        std::shared_ptr<ITexture> m_pMetallicRoughnessMap;
        float m_metallic;
        float m_roughness;

        std::shared_ptr<ITexture> m_pNormalMap;
        std::shared_ptr<ITexture> m_pHeightMap;
        std::shared_ptr<ITexture> m_pOcclusionMap;

        std::shared_ptr<ITexture> m_pEmissiveMap;
        float3 m_emissive;
    };
}