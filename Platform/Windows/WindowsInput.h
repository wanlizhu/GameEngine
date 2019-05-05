#pragma once

#include "InputSystem.h"

namespace Platform
{
    enum EInputControlMsg_Win
    {
        eEm_InputControl_Null = 0,
        eEm_InputControl_Char = 1,
        eEm_InputControl_Back = 2,
        eEm_InputControl_Tab = 3,
        eEm_InputControl_Clear = 4,
        eEm_InputControl_Return = 5,
        eEm_InputControl_Shift = 6,
        eEm_InputControl_Control = 7,
        eEm_InputControl_Menu =  8,
        eEm_InputControl_Pause = 9,
        eEm_InputControl_Capital = 10,
        eEm_InputControl_Escape = 11,
        eEm_InputControl_Space = 12,
        eEm_InputControl_Prior = 13,
        eEm_InputControl_Next = 14,
        eEm_InputControl_End = 15,
        eEm_InputControl_Home = 16,
        eEm_InputControl_Left = 17,
        eEm_InputControl_Up = 18,
        eEm_InputControl_Right = 19,
        eEm_InputControl_Down = 20,
        eEm_InputControl_Select = 21,
        eEm_InputControl_Print = 22,
        eEm_InputControl_Execute = 23,
        eEm_InputControl_Snapshot = 24,
        eEm_InputControl_Insert = 25,
        eEm_InputControl_Delete = 26,
        eEm_InputControl_Help = 27,
        eEm_InputControl_LWin = 28,
        eEm_InputControl_RWin = 29,
        eEm_InputControl_Apps = 30,
        eEm_InputControl_F1 = 31,
        eEm_InputControl_F2 = 32,
        eEm_InputControl_F3 = 33,
        eEm_InputControl_F4 = 34,
        eEm_InputControl_F5 = 35,
        eEm_InputControl_F6 = 36,
        eEm_InputControl_F7 = 37,
        eEm_InputControl_F8 = 38,
        eEm_InputControl_F9 = 39,
        eEm_InputControl_F10 = 40,
        eEm_InputControl_F11 = 41,
        eEm_InputControl_F12 = 42,
    };

    class WindowsInput : public Engine::InputSystem
    {
    public:
        WindowsInput() {}
        virtual ~WindowsInput() {}

        void Initialize() override;
        void Shutdown() override;
        void Tick() override;

        static void PeekWindowsInputMessage(UINT message, WPARAM wParam, LPARAM lParam);
    };
}