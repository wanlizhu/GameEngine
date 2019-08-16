#pragma once

#include <vector>

#include "IAnimationSystem.h"
#include "AnimationComponent.h"

#include "ECSSystem.h"

namespace Engine
{
    class AnimationComponent;
    class AnimationSystem : public IAnimationSystem, public ECSSystemBase<AnimationComponent>
    {
    public:
        AnimationSystem();
        virtual ~AnimationSystem();

        void Initialize() override;
        void Shutdown() override;
        void Tick(float elapsedTime) override;

        void FlushEntity(std::shared_ptr<IEntity> pEntity) override;

    private:
        std::vector<std::shared_ptr<IEntity>> m_pEntityList;

    public:
        static std::vector<AnimationFunc> s_cbTables;
    };
}