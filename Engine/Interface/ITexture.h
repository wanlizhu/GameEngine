#pragma once

#include <memory>
#include <string>

#include "DrawingDevice.h"

namespace Engine
{
    class ITexture
    {
    public:
        ITexture() {}
        virtual ~ITexture() = default;

        virtual std::string GetURI() const = 0;
        virtual void SetURI(std::string uri) = 0;

        virtual std::shared_ptr<DrawingTexture> GetTexture() const = 0;
        virtual void SetTexture(std::shared_ptr<DrawingTexture> pTexture) = 0;
    };
}