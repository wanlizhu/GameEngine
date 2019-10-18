#include "StandardMaterial.h"

using namespace Engine;

StandardMaterial::StandardMaterial() :
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

StandardMaterial::~StandardMaterial()
{
}

std::shared_ptr<ITexture> StandardMaterial::GetAlbedoMap() const
{
    return m_pAlbedoMap;
}

void StandardMaterial::SetAlbedoMap(std::shared_ptr<ITexture> map)
{
    m_pAlbedoMap = map;
}

float4 StandardMaterial::GetAlbedoColor() const
{
    return m_albedoColor;
}

void StandardMaterial::SetAlbedoColor(float4& color)
{
    m_albedoColor = color;
}

std::shared_ptr<ITexture> StandardMaterial::GetMetallicRoughnessMap() const
{
    return m_pMetallicRoughnessMap;
}

void StandardMaterial::SetMetallicRoughnessMap(std::shared_ptr<ITexture> map)
{
    m_pMetallicRoughnessMap = map;
}

float StandardMaterial::GetMetallic() const
{
    return m_metallic;
}

void StandardMaterial::SetMetallic(float metallic)
{
    m_metallic = metallic;
}

float StandardMaterial::GetRoughness() const
{
    return m_roughness;
}

void StandardMaterial::SetRoughness(float roughness)
{
    m_roughness = roughness;
}

std::shared_ptr<ITexture> StandardMaterial::GetNormalMap() const
{
    return m_pNormalMap;
}

void StandardMaterial::SetNormalMap(std::shared_ptr<ITexture> map)
{
    m_pNormalMap = map;
}

std::shared_ptr<ITexture> StandardMaterial::GetHeightMap() const
{
    return m_pNormalMap;
}

void StandardMaterial::SetHeightMap(std::shared_ptr<ITexture> map)
{
    m_pNormalMap = map;
}

std::shared_ptr<ITexture> StandardMaterial::GetOcclusionMap() const
{
    return m_pOcclusionMap;
}

void StandardMaterial::SetOcclusionMap(std::shared_ptr<ITexture> map)
{
    m_pOcclusionMap = map;
}

std::shared_ptr<ITexture> StandardMaterial::GetEmissiveMap() const
{
    return m_pEmissiveMap;
}

void StandardMaterial::SetEmissiveMap(std::shared_ptr<ITexture> map)
{
    m_pEmissiveMap = map;
}

float3 StandardMaterial::GetEmissive() const
{
    return m_emissive;
}

void StandardMaterial::SetEmissive(float* emissive)
{
    m_emissive = float3(emissive[0], emissive[1], emissive[2]);
}