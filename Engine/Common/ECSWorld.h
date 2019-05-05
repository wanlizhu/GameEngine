#pragma once

#include "IECSWorld.h"

namespace Engine
{
    class ECSWorld : public IECSWorld
    {
    public:
        ECSWorld() = default;

        void Initialize() override;
        void Shutdown() override;

        void Tick() override;

    private:
        void Flush();

    protected:
        std::shared_ptr<IEntity> CreateEntity(const std::vector<IComponent*>& pComponents, const std::vector<CompID>& ids) override;
    };
}
