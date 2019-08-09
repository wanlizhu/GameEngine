#include "AnimationComponent.h"

using namespace Engine;

void AnimationComponent::SetAnimationFunc(AnimationComponent::AnimationFunc func)
{
    m_animFunc = func;
}

const AnimationComponent::AnimationFunc& AnimationComponent::GetAnimationFunc()
{
    return m_animFunc;
}

void AnimationComponent::Apply(float elapsedTime)
{
    m_animFunc(elapsedTime);
}