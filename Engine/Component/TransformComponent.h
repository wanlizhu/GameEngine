#pragma once

#include "Vector.h"

#include "Component.h"

namespace Engine
{
    class TransformComponent : public ComponentBase<TransformComponent>
    {
    public:
        TransformComponent();
        virtual ~TransformComponent() = default;

        float3 GetPosition() const;
        void SetPosition(float3& pos);

        float3 GetRotate() const;
        void SetRotate(float3& rotate);

        float4 GetQuaternion() const;
        void SetQuaternion(float4& quaternion);

        float3 GetScale() const;
        void SetScale(float3& scale);

    private:
        float3 m_position;
        float3 m_rotate;
        float4 m_quaternion;
        float3 m_scale;
    };
}