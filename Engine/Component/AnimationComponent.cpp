#include <assert.h>

#include "AnimationComponent.h"
#include "AnimationSystem.h"

using namespace Engine;

void AnimationComponent::SetAnimationFunc(AnimationFunc& func)
{
    auto size = AnimationSystem::s_cbTables.size();
    m_cbID = (uint32_t)size;
    AnimationSystem::s_cbTables.emplace_back(func);
}

const AnimationFunc& AnimationComponent::GetAnimationFunc()
{
    return AnimationSystem::s_cbTables[m_cbID];
}

void AnimationComponent::Apply(float elapsedTime)
{
    auto func = GetAnimationFunc();
    func(elapsedTime);
}