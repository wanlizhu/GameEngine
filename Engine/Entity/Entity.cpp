#include "Entity.h"
#include "Algorithm.h"

using namespace Engine;

EntityBase::EntityBase(std::shared_ptr<IECSWorld> pWorld) : IEntity(pWorld)
{
}

EntityBase::~EntityBase()
{
}

void EntityBase::AttachComponent(CompID compId, IComponent* pComponent, std::unordered_map<CompID, std::vector<uint8_t>>& memory)
{
    auto createFunc = pComponent->GetCreateFunc(compId);
    auto compIndex = createFunc(pComponent, std::shared_ptr<IEntity>(this), memory[compId]);
    m_comps.emplace_back(std::pair<CompID, CompIndex>(compId, compIndex));
    m_compBitset = AddBit<CompBitset>(m_compBitset, compId);
}

void EntityBase::DetachComponent(CompID compId, CompIndex compIndex, std::unordered_map<CompID, std::vector<uint8_t>>& memory)
{
    auto size = IComponent::GetSize(compId);
    auto indexLast = memory[compId].size() - size;

    auto destroyFunc = IComponent::GetDestroyFunc(compId);
    auto pComp = (IComponent*)&memory[compId][compIndex];
    auto pCompLast = (IComponent*)&memory[compId][indexLast];
    destroyFunc(pComp);

    if (compIndex == indexLast)
    {
        memory[compId].resize(indexLast);
        return;
    }

    memcpy(pComp, pCompLast, size);
    auto pEntity = std::dynamic_pointer_cast<EntityBase>(pCompLast->m_pEntity);
    auto& lastComponenets = pEntity->m_comps;
    for (uint32_t i = 0; i < lastComponenets.size(); i++)
    {
        if (lastComponenets[i].first == compId && lastComponenets[i].second == indexLast)
        {
            lastComponenets[i].second = compIndex;
            break;
        }
    }
    memory[compId].resize(indexLast);
    m_compBitset = ClearBit<CompBitset>(m_compBitset, compId);
}