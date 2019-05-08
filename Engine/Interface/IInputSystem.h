#pragma once

#include <iostream>

#include "IRuntimeModule.h"
#include "IEventSystem.h"

namespace Engine
{
    enum EInputEvent
    {
        eEv_Input_Unknown = 100,

        eEv_Input_KeyChar = 101,
        eEv_Input_KeyDown = 102,
        eEv_Input_KeyUp = 103,
        eEv_Input_KeyDbClick = 104,

        eEv_Input_ControlMove = 111,
        eEv_Input_ControlLeave = 112,
        eEv_Input_ControlWheel = 113,
        eEv_Input_ControlHover = 114,
    };

    class InputMsg
    {
    public:
        InputMsg(uint32_t _ctrID = 0, int64_t _param1 = 0, int64_t _param2 = 0) : ctrID(_ctrID), param1(_param1), param2(_param2) {}
        uint32_t CtrID() const { return ctrID; }
        int64_t Param1() const { return param1; }
        int64_t Param2() const { return param2; }

        void setCtrID(uint32_t id) { ctrID = id; }
        void setParam1(int64_t p) { param1 = p; }
        void setParam2(int64_t p) { param2 = p; }

        friend std::ostream& operator<< (std::ostream &out, InputMsg const& e)
        {
            return out << "(ctrID: " << e.ctrID << ", param1: " << e.param1 << ", param2: " << e.param2 << ")";
        }

    private:
        uint32_t ctrID;
        int64_t param1;
        int64_t param2;

    };


    class IInputSystem : public IRuntimeModule
    {
    public:
        virtual ~IInputSystem() {}

        virtual void Initialize() override = 0;
        virtual void Shutdown() override = 0;

        virtual void Tick(float elapsedTime) override = 0;

        virtual void DispatchInputEvent(EInputEvent event, InputMsg msg) = 0;
    };
}

