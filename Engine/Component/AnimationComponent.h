#pragma once

#include <functional>

#include "Component.h"

namespace Engine
{
    class AnimationComponent : public ComponentBase<AnimationComponent>
    {
    public:
        AnimationComponent() : ComponentBase<AnimationComponent>() {}
        virtual ~AnimationComponent() = default;

        typedef std::function<void(float)> AnimationFunc;

        void SetAnimationFunc(AnimationFunc func);
        const AnimationFunc& GetAnimationFunc();

        void Apply(float elapsedTime);

    private:
        AnimationFunc m_animFunc;
    };
}