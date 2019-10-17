#include <algorithm>

#include "ECSWorld.h"
#include "Entity.h"

using namespace Engine;

void ECSWorld::Initialize()
{
    IECSWorld::Initialize();
    Flush();

    m_bEntityChanged = false;
}

void ECSWorld::Shutdown()
{
    IECSWorld::Shutdown();
}

void ECSWorld::Tick(float elapsedTime)
{
    Flush();
    IECSWorld::Tick(elapsedTime);
}

std::shared_ptr<IEntity> ECSWorld::CreateEntity(const std::vector<IComponent*>& pComponents, const std::vector<CompID>& ids)
{
    std::shared_ptr<IEntity> pEntity = std::make_shared<EntityBase>(std::shared_ptr<IECSWorld>(this));
    pEntity->m_id = (EntityID)m_entityPool.size();

    for (uint32_t i = 0; i < pComponents.size(); i++)
        pEntity->AttachComponent(ids[i], pComponents[i], m_compTable);

    m_entityPool.emplace_back(pEntity);
    return pEntity;
}

void ECSWorld::Flush()
{
    if (!m_bEntityChanged)
        return;

    for (auto& system : m_systemPool)
        for (auto& entity : m_entityPool)
            system->FlushEntity(entity);
}