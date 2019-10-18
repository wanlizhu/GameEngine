#include "Global.h"
#include "EventSystem.h"

using namespace Engine;

void EventSystem::Initialize()
{
}

void EventSystem::Shutdown()
{
    m_eventQueue.clear();
    for (auto& it : m_eventListeners)
    {
        auto & list = it.second;
        list.clear();
    }
    m_eventListeners.clear();
}

void EventSystem::Tick(float elapsedTime)
{
    ProcessEvents();
}

void EventSystem::FlushEntity(IEntity* pEntity)
{
}

bool EventSystem::AddListener(IEventData::id_t id, EventDelegate proc)
{
    auto it = m_eventListeners.find(id);
    if (it == m_eventListeners.end())
        m_eventListeners[id] = EventDelegateList();
    
    auto &list = m_eventListeners[id];
    for (auto &p : list)
    {
        if (p.target<EventDelegate>() == proc.target<EventDelegate>())
            return false;
    }
    list.emplace_back(proc);

    return true;
}

bool EventSystem::RemoveListener(IEventData::id_t id, EventDelegate proc)
{
    auto it = m_eventListeners.find(id);
    if (it == m_eventListeners.end())
        return false;

    auto &list = it->second;
    for (auto it = list.cbegin(); it != list.cend(); ++it)
    {
        const EventDelegate &p = *it; 
        if (p.target<EventDelegate>() == proc.target<EventDelegate>())
        {
            list.erase(it);
            return true;
        }
    }
    return false;
}

void EventSystem::QueueEvent(IEventDataPtr e)
{
    m_eventQueue.emplace_back(e);
}

void EventSystem::ProcessEvents()
{
    for (auto it = m_eventQueue.cbegin(); it != m_eventQueue.cend();)
    {
        auto &e = *it;
        auto listeners = m_eventListeners.find(e->GetID());
        if (listeners != m_eventListeners.cend())
        {
            for (auto p : listeners->second)
                p(e);
        }
        it = m_eventQueue.erase(it);
    }
}

EventListener::EventListener()
{
    el_mEventSystem = gpGlobal->GetEventSystem();
}

EventListener::~EventListener()
{
    if (el_mEventSystem.expired())
        return;
    auto em = el_mEventSystem.lock();
    for (auto &e : el_mEvent)
        em->RemoveListener(e.first, e.second);
}

bool EventListener::OnEvent(IEventData::id_t id, EventDelegate proc)
{
    if(el_mEventSystem.expired())
        return false; 
    auto em = el_mEventSystem.lock(); 
    if (em->AddListener(id, proc)){
        el_mEvent.emplace_back(id, proc); 
    }
    return true;
}

bool EventListener::Dispatch(IEventData::id_t id, EventDelegate proc)
{
    if(el_mEventSystem.expired())
        return false; 
    auto em = el_mEventSystem.lock(); 
    em->RemoveListener(id, proc);
    return true;
}