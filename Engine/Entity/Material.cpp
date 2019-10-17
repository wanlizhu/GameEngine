#include "Material.h"

using namespace Engine;

Material::Material() :
    m_pAlbedoMap(nullptr),
    m_albedoColor(float4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_pMetallicRoughnessMap(nullptr),
    m_metallic(0.0f),
    m_roughness(0.0f),
    m_pNormalMap(nullptr),
    m_pHeightMap(nullptr),
    m_pOcclusionMap(nullptr)
{
}

Material::~Material()
{
}

std::shared_ptr<ITexture> Material::GetAlbedoMap() const
{
    return m_pAlbedoMap;
}

void Material::SetAlbedoMap(std::shared_ptr<ITexture> map)
{
    m_pAlbedoMap = map;
}

float4 Material::GetAlbedoColor() const
{
    return m_albedoColor;
}

void Material::SetAlbedoColor(float4& color)
{
    m_albedoColor = color;
}

std::shared_ptr<ITexture> Material::GetMetallicRoughnessMap() const
{
    return m_pMetallicRoughnessMap;
}

void Material::SetMetallicRoughnessMap(std::shared_ptr<ITexture> map)
{
    m_pMetallicRoughnessMap = map;
}

float Material::GetMetallic() const
{
    return m_metallic;
}

void Material::SetMetallic(float metallic)
{
    m_metallic = metallic;
}

float Material::GetRoughness() const
{
    return m_roughness;
}

void Material::SetRoughness(float roughness)
{
    m_roughness = roughness;
}

std::shared_ptr<ITexture> Material::GetNormalMap() const
{
    return m_pNormalMap;
}

void Material::SetNormalMap(std::shared_ptr<ITexture> map)
{
    m_pNormalMap = map;
}

std::shared_ptr<ITexture> Material::GetHeightMap() const
{
    return m_pNormalMap;
}

void Material::SetHeightMap(std::shared_ptr<ITexture> map)
{
    m_pNormalMap = map;
}

std::shared_ptr<ITexture> Material::GetOcclusionMap() const
{
    return m_pOcclusionMap;
}

void Material::SetOcclusionMap(std::shared_ptr<ITexture> map)
{
    m_pOcclusionMap = map;
}

std::shared_ptr<ITexture> Material::GetEmissiveMap() const
{
    return m_pEmissiveMap;
}

void Material::SetEmissiveMap(std::shared_ptr<ITexture> map)
{
    m_pEmissiveMap = map;
}

float3 Material::GetEmissive() const
{
    return m_emissive;
}

void Material::SetEmissive(float* emissive)
{
    m_emissive = float3(emissive[0], emissive[1], emissive[2]);
}