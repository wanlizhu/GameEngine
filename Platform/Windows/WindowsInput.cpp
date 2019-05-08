#include <Windows.h>

#include "Global.h"
#include "IInputSystem.h"
#include "WindowsInput.h"

using namespace Engine;
using namespace Platform;

void WindowsInput::Initialize()
{
}

void WindowsInput::Shutdown()
{
}

void WindowsInput::Tick(float elapsedTime)
{
}

void WindowsInput::PeekWindowsInputMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    auto pInputSystem = gpGlobal->GetInputSystem();
    InputMsg msg;
    switch (message)
    {
        case WM_CHAR:
        {
            msg.setCtrID(eEm_InputControl_Char);
            msg.setParam1((int64_t)wParam);
            pInputSystem->DispatchInputEvent(eEv_Input_KeyChar, msg);
            break;
        }
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            bool bCatched = true;
            switch (wParam)
            {
                case VK_ESCAPE:
                    msg.setCtrID(eEm_InputControl_Escape);
                    break; 
                case VK_TAB:
                    msg.setCtrID(eEm_InputControl_Tab);
                    break;
                case VK_SHIFT:
                    msg.setCtrID(eEm_InputControl_Shift);
                    break;
                case VK_CONTROL:
                    msg.setCtrID(eEm_InputControl_Control);
                    break;
                case VK_SPACE:
                    msg.setCtrID(eEm_InputControl_Space);
                    break;
                case VK_LEFT:
                    msg.setCtrID(eEm_InputControl_Left);
                    break;
                case VK_UP:
                    msg.setCtrID(eEm_InputControl_Up);
                    break;
                case VK_RIGHT:
                    msg.setCtrID(eEm_InputControl_Right);
                    break;
                case VK_DOWN:
                    msg.setCtrID(eEm_InputControl_Down);
                    break;
                default:
                    bCatched = false;
            }

            if (bCatched)
            {
                if (message == WM_KEYUP)
                    pInputSystem->DispatchInputEvent(eEv_Input_KeyUp, msg);
                else if (message == WM_KEYDOWN)
                    pInputSystem->DispatchInputEvent(eEv_Input_KeyDown, msg);
            }
            break;
        }
    }
}