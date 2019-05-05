#pragma once

#include <memory>
#include <list>
#include <vector>
#include <string>
#include <functional>
#include <stdint.h>

#include "IRuntimeModule.h"
#include "Traits.h"
#include "Global.h"

namespace Engine
{
    class IEventData
    {
    public:
        typedef uint32_t id_t;
        typedef std::string name_t;

        virtual id_t GetID() const = 0;
        virtual name_t GetName() const = 0;
    };

    template <typename T, typename E, E e,
              typename = typename std::enable_if<std::is_enum<E>::value>::type>
    class Event : public IEventData
    {
    public:
        typedef T msg_t;

        Event() {}
        Event(const msg_t t, const char* n) : m_id(e), m_name(n), m_msg(t) {}

        id_t GetID() const override     { return m_id; }
        name_t GetName() const override { return m_name; }
        msg_t GetMsg() const            { return m_msg; }

    private:
        id_t m_id;
        name_t m_name;
        msg_t m_msg;
    };

    typedef std::shared_ptr<IEventData> IEventDataPtr;
    typedef std::function<void(const IEventDataPtr&)> EventDelegate;
    typedef std::list<EventDelegate> EventDelegateList;
    typedef uintptr_t EventListenerAddr;

    class IEventSystem
    {
    public:
        virtual bool AddListener(IEventData::id_t id, EventDelegate proc) = 0;
        virtual bool RemoveListener(IEventData::id_t id, EventDelegate proc) = 0;
        virtual void QueueEvent(IEventDataPtr e) = 0;
        virtual void ProcessEvents() = 0;
    };

    class EventListener
    {
    public:
        EventListener();
        virtual ~EventListener();

        template<class T>
        bool OnEvent(IEventData::id_t id, std::function<void(std::shared_ptr<T>)> proc)
        {
            return OnEvent(id, [&, proc](IEventDataPtr data){
                auto ev = std::dynamic_pointer_cast<T>(data); 
                if(ev) proc(ev);
            }); 
        }
        template<class T>
        bool Dispatch(IEventData::id_t id, std::function<void(std::shared_ptr<T>)> proc)
        {
            return Dispatch(id, [&, proc](IEventDataPtr data){
                auto ev = std::dynamic_pointer_cast<T>(data); 
                if(ev) proc(ev); 
            });
        }

    protected:
        typedef std::pair<IEventData::id_t, EventDelegate> EventPair;
        bool OnEvent(IEventData::id_t id, EventDelegate proc);
        bool Dispatch(IEventData::id_t id, EventDelegate proc);

    private:
        std::weak_ptr<IEventSystem> el_mEventSystem;
        std::vector<EventPair> el_mEvent;
    }; 

#define DECLARE_EVENT(id, event, msg)                                                                       \
    Event<decltype(msg), decltype(id), id> event(msg, #id)

#define EMITTER_EVENT(event)                                                                                \
    gpGlobal->GetEventSystem()->QueueEvent(std::shared_ptr<IEventData>(new decltype(event)(event)))

#define DECLARE_LISTENER()                                                                                  \
    EventListener listener

#define LISTEN_EVENT(id, func)                                                                              \
    {                                                                                                       \
        auto f = func;                                                                                      \
        typedef function_traits<decltype(f)> traits;                                                        \
        listener.OnEvent<traits::arg<0>::type::element_type>(id, func);                                     \
    }                                                                                                       \

#define DISPATCH_EVENT(id, func)                                                                            \
    {                                                                                                       \
        auto f = func;                                                                                      \
        typedef function_traits<decltype(f)> traits;                                                        \
        listener.Dispatch<traits::arg<0>::type::element_type>(id, func);                                    \
    }
}