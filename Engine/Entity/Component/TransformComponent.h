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

        Vec3<float> GetPosition() const;
        void SetPosition(Vec3<float>& pos);

        Vec3<float> GetRotate() const;
        void SetRotate(Vec3<float>& rotate);

        Vec3<float> GetScale() const;
        void SetScale(Vec3<float>& scale);

    private:
        Vec3<float> m_position;
        Vec3<float> m_rotate;
        Vec3<float> m_scale;
    };
}