#pragma once

#include "DrawingDevice.h"

namespace Engine
{
    class DrawingTextureTarget
    {
    public:
        DrawingTextureTarget(std::shared_ptr<DrawingDevice> pDevice);
        DrawingTextureTarget(std::shared_ptr<DrawingDevice> pDevice, std::shared_ptr<DrawingTarget> pTarget, std::shared_ptr<DrawingTexture> pTexture);

        virtual ~DrawingTextureTarget() = default;

        std::shared_ptr<DrawingTarget> GetTarget();
        std::shared_ptr<DrawingTexture> GetTexture();

        bool Initialize(uint32_t width, uint32_t height, EDrawingFormatType type);

    private:
        std::shared_ptr<DrawingDevice> m_pDevice;
        std::shared_ptr<DrawingTarget> m_pTarget;
        std::shared_ptr<DrawingTexture> m_pTexture;
    };
}