#include "TransformComponent.h"

using namespace Engine;

Vec3<float> TransformComponent::GetPosition() const
{
    return m_position;
}

void TransformComponent::SetPosition(Vec3<float>& pos)
{
    m_position = pos;
}

Vec3<float> TransformComponent::GetRotate() const
{
    return m_rotate;
}

void TransformComponent::SetRotate(Vec3<float>& rotate)
{
    m_rotate = rotate;
}

Vec3<float> TransformComponent::GetScale() const
{
    return m_scale;
}

void TransformComponent::SetScale(Vec3<float>& scale)
{
    m_scale = scale;
}