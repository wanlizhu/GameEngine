#include "CameraComponent.h"

using namespace Engine;

CameraComponent::CameraComponent(ERendererType rendererType,
                                 EProjectionType projType,
                                 EClearType clearType,
                                 float4& color,
                                 float fov,
                                 float near,
                                 float far) :
    m_rendererType(rendererType),
    m_projType(projType),
    m_clearType(clearType),
    m_background(color),
    m_fov(fov),
    m_clippingNear(near),
    m_clippingFar(far)
{
}

ERendererType CameraComponent::GetRendererType() const
{
    return m_rendererType;
}

void CameraComponent::SetRendererType(ERendererType type)
{
    m_rendererType = type;
}

EProjectionType CameraComponent::GetProjectionType() const
{
    return m_projType;
}

void CameraComponent::SetProjectionType(EProjectionType type)
{
    m_projType = type;
}

EClearType CameraComponent::GetClearType() const
{
    return m_clearType;
}

void CameraComponent::SetClearType(EClearType type)
{
    m_clearType = type;
}

float4 CameraComponent::GetBackground() const
{
    return m_background;
}

void CameraComponent::SetBackground(float4& color)
{
    m_background = color;
}

float CameraComponent::GetFov() const
{
    return m_fov;
}

void CameraComponent::SetFov(float fov)
{
    m_fov = fov;
}

float CameraComponent::GetClippingNear() const
{
    return m_clippingNear;
}

void CameraComponent::SetClippingNear(float near)
{
    m_clippingNear = near;
}

float CameraComponent::GetClippingFar() const
{
    return m_clippingFar;
}

void CameraComponent::SetClippingFar(float far)
{
    m_clippingFar = far;
}