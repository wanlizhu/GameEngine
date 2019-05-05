#include "IECSWorld.h"

namespace Engine
{
    class EntityBase : public IEntity
    {
    public:
        EntityBase(std::shared_ptr<IECSWorld> pWorld);
        virtual ~EntityBase();

        void AttachComponent(CompID compId, IComponent* pComponent, std::unordered_map<CompID, std::vector<uint8_t>>& memory) override;
        void DetachComponent(CompID compId, CompIndex compIndex, std::unordered_map<CompID, std::vector<uint8_t>>& memory) override;
    };
}