#include "TransformComponent.h"

using namespace Engine;

Vec3<float>& TransformComponent::GetPosition()
{
    return m_position;
}

void TransformComponent::SetPosition(Vec3<float>& pos)
{
    m_position = pos;
}