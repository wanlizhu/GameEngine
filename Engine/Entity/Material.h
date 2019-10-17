#pragma once

#include "Vector.h"
#include "IMaterial.h"

namespace Engine
{
    class Material : public IMaterial
    {
    public:
        Material();
        virtual ~Material();

        std::shared_ptr<ITexture> GetAlbedoMap() const override;
        void SetAlbedoMap(std::shared_ptr<ITexture> map) override;

        float4 GetAlbedoColor() const override;
        void SetAlbedoColor(float4& color) override;

        std::shared_ptr<ITexture> GetMetallicRoughnessMap() const override;
        void SetMetallicRoughnessMap(std::shared_ptr<ITexture> map) override;

        float GetMetallic() const override;
        void SetMetallic(float metallic) override;

        float GetRoughness() const override;
        void SetRoughness(float roughness) override;

        std::shared_ptr<ITexture> GetNormalMap() const override;
        void SetNormalMap(std::shared_ptr<ITexture> map) override;

        std::shared_ptr<ITexture> GetHeightMap() const override;
        void SetHeightMap(std::shared_ptr<ITexture> map) override;

        std::shared_ptr<ITexture> GetOcclusionMap() const override;
        void SetOcclusionMap(std::shared_ptr<ITexture> map) override;

        std::shared_ptr<ITexture> GetEmissiveMap() const override;
        void SetEmissiveMap(std::shared_ptr<ITexture> map) override;

        float3 GetEmissive() const override;
        void SetEmissive(float* emissive) override;

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