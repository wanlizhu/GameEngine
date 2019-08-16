#pragma once

#include <functional>
#include <vector>

#include "Component.h"

namespace Engine
{
    typedef std::function<void(float)> AnimationFunc;

    class AnimationComponent : public ComponentBase<AnimationComponent>
    {
    public:
        AnimationComponent() : ComponentBase<AnimationComponent>() {}
        virtual ~AnimationComponent() = default;

        void SetAnimationFunc(AnimationFunc& func);
        const AnimationFunc& GetAnimationFunc();

        void Apply(float elapsedTime);

    private:
        uint32_t m_cbID = 0;
    };
}