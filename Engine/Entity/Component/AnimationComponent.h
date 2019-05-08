#pragma once

#include "Component.h"

namespace Engine
{
    class AnimationComponent : public ComponentBase<AnimationComponent>
    {
    public:
        AnimationComponent() : ComponentBase<AnimationComponent>() {}
        virtual ~AnimationComponent() = default;
    };
}