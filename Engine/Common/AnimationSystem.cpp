#include <assert.h>

#include "Global.h"
#include "AnimationComponent.h"

#include "AnimationSystem.h"

using namespace Engine;

AnimationSystem::AnimationSystem()
{
}

AnimationSystem::~AnimationSystem()
{
}

void AnimationSystem::Initialize()
{
}

void AnimationSystem::Shutdown()
{
}

void AnimationSystem::Tick(float elapsedTime)
{
    for(auto& pEntity : m_pEntityList)
    {
        auto pAnimationComponent = pEntity->GetComponent<AnimationComponent>();
        assert(pAnimationComponent != nullptr);
        pAnimationComponent->Apply(elapsedTime);
    }
}

void AnimationSystem::FlushEntity(std::shared_ptr<IEntity> pEntity)
{
    if (pEntity->HasComponent<AnimationComponent>())
        m_pEntityList.emplace_back(pEntity);
}