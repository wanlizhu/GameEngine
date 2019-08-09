#include "TransformComponent.h"

using namespace Engine;

TransformComponent::TransformComponent() : ComponentBase<TransformComponent>(),
    m_scale(1.0f, 1.0f, 1.0f)
{
}

float3 TransformComponent::GetPosition() const
{
    return m_position;
}

void TransformComponent::SetPosition(float3& pos)
{
    m_position = pos;
}

float3 TransformComponent::GetRotate() const
{
    return m_rotate;
}

void TransformComponent::SetRotate(float3& rotate)
{
    m_rotate = rotate;
}

float3 TransformComponent::GetScale() const
{
    return m_scale;
}

void TransformComponent::SetScale(float3& scale)
{
    m_scale = scale;
}