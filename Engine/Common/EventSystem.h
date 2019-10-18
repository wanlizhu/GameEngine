#pragma once

#include <map>
#include <memory>

#include "IEventSystem.h"

#include "ECSSystem.h"

namespace Engine
{
    class EventSystem : public IEventSystem, public ECSSystemBase<>
    {
    public:
        EventSystem() {}
        virtual ~EventSystem() {}

        void Initialize() override;
        void Shutdown() override;
        void Tick(float elapsedTime) override;

        void FlushEntity(IEntity* pEntity) override;

        bool AddListener(IEventData::id_t id, EventDelegate proc) override;
        bool RemoveListener(IEventData::id_t id, EventDelegate proc) override;
        void QueueEvent(IEventDataPtr e) override;

    protected:
        void ProcessEvents() override;

    private:
        std::list<IEventDataPtr> m_eventQueue;
        std::map<IEventData::id_t, EventDelegateList> m_eventListeners;
    };
}