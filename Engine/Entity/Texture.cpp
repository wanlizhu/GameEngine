#include "Texture.h"

using namespace Engine;

Texture::Texture()
{
    m_pTexture = nullptr;
}

Texture::Texture(std::string uri) :
    m_uri(uri)
{
    m_pTexture = nullptr;
}

Texture::~Texture()
{
}

std::string Texture::GetURI() const
{
    return m_uri;
}

void Texture::SetURI(std::string uri)
{
    m_uri = uri;
}

std::shared_ptr<DrawingTexture> Texture::GetTexture() const
{
    return m_pTexture;
}

void Texture::SetTexture(std::shared_ptr<DrawingTexture> pTexture)
{
    m_pTexture = pTexture;
}