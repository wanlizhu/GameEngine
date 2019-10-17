#pragma once

#include <string>

#include "DrawingDevice.h"
#include "ITexture.h"

namespace Engine
{
    class Texture : public ITexture
    {
    public:
        Texture();
        Texture(std::string uri);
        virtual ~Texture();

        std::string GetURI() const override;
        void SetURI(std::string uri) override;

        std::shared_ptr<DrawingTexture> GetTexture() const override;
        void SetTexture(std::shared_ptr<DrawingTexture> pTexture) override;

    protected:
        std::string m_uri;
        std::shared_ptr<DrawingTexture> m_pTexture;
    };
}