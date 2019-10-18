#pragma once

#include "ISceneSystem.h"

#include "ECSSystem.h"

namespace Engine
{
    class TransformComponent;
    class SceneSystem: public ISceneSystem, public ECSSystemBase<TransformComponent>
    {
    public:
        SceneSystem() {}
        virtual ~SceneSystem() {}

        void Initialize() override;
        void Shutdown() override;
        void Tick(float elapsedTime) override;

        void FlushEntity(IEntity* pEntity) override;
    };
}