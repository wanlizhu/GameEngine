#pragma once

#include "Vec3.h"

#include "Component.h"

namespace Engine
{
    class TransformComponent : public ComponentBase<TransformComponent>
    {
    public:
        TransformComponent() : ComponentBase<TransformComponent>() {}
        virtual ~TransformComponent() = default;

        Vec3<float>& GetPosition();
        void SetPosition(Vec3<float>& pos);

    private:
        Vec3<float> m_position;
    };
}