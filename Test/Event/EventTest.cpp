#include <string>
#include <memory>
#include <iostream>

#include "Global.h"
#include "IEventSystem.h"
#include "EventSystem.h"

using namespace Engine;

enum ETestEvent
{
    eTestEvent_1 = 1,
    eTestEvent_2 = 2,
    eTestEvent_3 = 3
};

struct EventData
{
    EventData(int _val, std::string _data) : val(_val), data(_data) {}

    int val;
    std::string data;

    friend std::ostream& operator<< (std::ostream &out, EventData const& d)
    {
        return out << "(val: " << d.val << ", data: " << d.data << ")";
    }
};

int main()
{
    if (gpGlobal == nullptr)
        gpGlobal = new Global();

    gpGlobal->RegisterRuntimeModule<EventSystem>(eSystem_Event);

    DECLARE_LISTENER();
    {
        LISTEN_EVENT(eTestEvent_1, [&](std::shared_ptr<Event<std::string, ETestEvent, eTestEvent_1>> data){
            std::cout << "Listen 1: " << data->GetMsg() << std::endl;
        });
        LISTEN_EVENT(eTestEvent_2, [&](std::shared_ptr<Event<EventData, ETestEvent, eTestEvent_2>> data){
            std::cout << "Listen 2: " << data->GetMsg() << std::endl;
        });
        LISTEN_EVENT(eTestEvent_3, [&](std::shared_ptr<Event<EventData, ETestEvent, eTestEvent_3>> data){
            std::cout << "Listen 3: " << data->GetMsg() << std::endl;
        });
    }

    {
        DECLARE_EVENT(eTestEvent_1, Test_1_Ev, std::string("This is Test_1_Ev"));

        DECLARE_EVENT(eTestEvent_2, Test_2_1_Ev, EventData(201, "This is Test_2_1_Ev"));
        DECLARE_EVENT(eTestEvent_2, Test_2_2_Ev, EventData(202, "This is Test_2_2_Ev"));
        DECLARE_EVENT(eTestEvent_2, Test_2_3_Ev, EventData(202, "This is Test_2_3_Ev"));

        DECLARE_EVENT(eTestEvent_3, Test_3_Ev, EventData(300, "This is Test_3_Ev"));

        for(int i = 0; i < 2; i++)
            EMITTER_EVENT(Test_1_Ev);

        EMITTER_EVENT(Test_2_1_Ev);
        EMITTER_EVENT(Test_2_2_Ev);

        EMITTER_EVENT(Test_3_Ev);
    }

    gpGlobal->GetEventSystem()->ProcessEvents();

    return 0;
}