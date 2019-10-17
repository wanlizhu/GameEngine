#include "DrawingTextureTarget.h"

using namespace Engine;

DrawingTextureTarget::DrawingTextureTarget(std::shared_ptr<DrawingDevice> pDevice) :
    m_pDevice(pDevice), m_pTarget(nullptr), m_pTexture(nullptr)
{
}

DrawingTextureTarget::DrawingTextureTarget(std::shared_ptr<DrawingDevice> pDevice, std::shared_ptr<DrawingTarget> pTarget, std::shared_ptr<DrawingTexture> pTexture) :
    m_pDevice(pDevice), m_pTarget(pTarget), m_pTexture(pTexture)
{
}

std::shared_ptr<DrawingTarget> DrawingTextureTarget::GetTarget() const
{
    return m_pTarget;
}

std::shared_ptr<DrawingTexture> DrawingTextureTarget::GetTexture() const
{
    return m_pTexture;
}

bool DrawingTextureTarget::Initialize(uint32_t width, uint32_t height, EDrawingFormatType type)
{
    DrawingTargetDesc desc;
    desc.mWidth = width;
    desc.mHeight = height;
    desc.mSlices = 1;
    desc.mFormat = type;
    desc.mFlags = 0;

    std::shared_ptr<DrawingTarget> pTarget = nullptr;
    std::shared_ptr<DrawingTexture> pTexture = nullptr;

    m_pDevice->CreateTarget(desc, pTarget);
    if (pTarget != nullptr)
        m_pDevice->CreateTexture(DrawingTextureDesc(), pTexture, pTarget);

    m_pTarget = pTarget;
    m_pTexture = pTexture;

    return (m_pTarget != nullptr) && (m_pTexture != nullptr);
}

DrawingTextureDepthBuffer::DrawingTextureDepthBuffer(std::shared_ptr<DrawingDevice> pDevice) :
    m_pDevice(pDevice), m_pDepthBuffer(nullptr), m_pTexture(nullptr)
{
}

DrawingTextureDepthBuffer::DrawingTextureDepthBuffer(std::shared_ptr<DrawingDevice> pDevice, std::shared_ptr<DrawingDepthBuffer> pDepthBuffer, std::shared_ptr<DrawingTexture> pTexture) :
    m_pDevice(pDevice), m_pDepthBuffer(pDepthBuffer), m_pTexture(pTexture)
{
}

std::shared_ptr<DrawingDepthBuffer> DrawingTextureDepthBuffer::GetDepthBuffer() const
{
    return m_pDepthBuffer;
}

std::shared_ptr<DrawingTexture> DrawingTextureDepthBuffer::GetTexture() const
{
    return m_pTexture;
}

bool DrawingTextureDepthBuffer::Initialize(uint32_t width, uint32_t height, EDrawingFormatType type)
{
    DrawingDepthBufferDesc desc;
    desc.mWidth = width;
    desc.mHeight = height;
    desc.mSlices = 1;
    desc.mFormat = type;
    desc.mFlags = 0;

    std::shared_ptr<DrawingDepthBuffer> pDepthBuffer = nullptr;
    std::shared_ptr<DrawingTexture> pTexture = nullptr;

    m_pDevice->CreateDepthBuffer(desc, pDepthBuffer);
    if (pDepthBuffer != nullptr)
        m_pDevice->CreateTexture(DrawingTextureDesc(), pTexture, pDepthBuffer);

    m_pDepthBuffer = pDepthBuffer;
    m_pTexture = pTexture;

    return (pDepthBuffer != nullptr) && (m_pTexture != nullptr);
}