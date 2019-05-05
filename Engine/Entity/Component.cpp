#include "Component.h"
#include "TransformComponent.h"

using namespace Engine;

IComponent::CompTableType IComponent::m_compTable;

uint32_t IComponent::RegisterComponent(CreateCompFunc createFunc, DestroyCompFunc destroyFunc, uint32_t size)
{
    CompID id = (CompID)m_compTable.size();
    m_compTable.emplace_back(CompType(createFunc, destroyFunc, size));
    return id;
}