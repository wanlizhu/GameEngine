#include "IInputSystem.h"
#include "EventSystem.h"
#include "InputSystem.h"

using namespace Engine;

std::ostream& operator<< (std::ostream &out, InputMsg const& e)
{
    return out << "(control ID: " << e.CtrID() << ", param 1: " << e.Param1() << ", param 2: " << e.Param2() << ")" << std::endl;
}

void InputSystem::Initialize()
{
}

void InputSystem::Shutdown()
{
    while (!m_inputQueue.empty())
        m_inputQueue.pop();
}

void InputSystem::Tick()
{
    while (!m_inputQueue.empty())
    {
        auto info = m_inputQueue.front();
        switch (info.event)
        {
            case eEv_Input_KeyChar:
            {
                DECLARE_EVENT(eEv_Input_KeyChar, InputKeyCharEv, info.msg);
                EMITTER_EVENT(InputKeyCharEv);
                break;
            }
            case eEv_Input_KeyDown:
            {
                DECLARE_EVENT(eEv_Input_KeyDown, InputKeyDownEv, info.msg);
                EMITTER_EVENT(InputKeyDownEv);
                break;
            }
            case eEv_Input_KeyUp:
            {
                DECLARE_EVENT(eEv_Input_KeyUp, InputKeyUpEv, info.msg);
                EMITTER_EVENT(InputKeyUpEv);
                break;
            }
            case eEv_Input_KeyDbClick:
            {
                DECLARE_EVENT(eEv_Input_KeyDbClick, InputKeyDbClickEv, info.msg);
                EMITTER_EVENT(InputKeyDbClickEv);
                break;
            }
            case eEv_Input_ControlMove:
            {
                DECLARE_EVENT(eEv_Input_ControlMove, InputControlMoveEv, info.msg);
                EMITTER_EVENT(InputControlMoveEv);
                break;
            }
            case eEv_Input_ControlLeave:
            {
                DECLARE_EVENT(eEv_Input_ControlLeave, InputControlLeaveEv, info.msg);
                EMITTER_EVENT(InputControlLeaveEv);
                break;
            }
            case eEv_Input_ControlWheel:
            {
                DECLARE_EVENT(eEv_Input_ControlWheel, InputControlWheelEv, info.msg);
                EMITTER_EVENT(InputControlWheelEv);
                break;
            }
            case eEv_Input_ControlHover:
            {
                DECLARE_EVENT(eEv_Input_ControlHover, InputControlHoverEv, info.msg);
                EMITTER_EVENT(InputControlHoverEv);
                break;
            }
            default:
            {
                DECLARE_EVENT(eEv_Input_Unknown, InputUnknownEv, info.msg);
                EMITTER_EVENT(InputUnknownEv);
                break;
            }
        }

        m_inputQueue.pop();
    }
}

void InputSystem::FlushEntity(std::shared_ptr<IEntity> pEntity)
{
}

void InputSystem::DispatchInputEvent(EInputEvent event, InputMsg msg)
{
    m_inputQueue.emplace(event, msg);
}