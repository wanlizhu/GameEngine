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
        void Tick() override;

        void FlushEntity(std::shared_ptr<IEntity> pEntity) override;
    };
}