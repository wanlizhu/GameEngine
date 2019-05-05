#pragma once

#include <Windows.h>
#include <memory>

#include "WindowsInput.h"
#include "BaseApplication.h"

namespace Platform
{
    class WindowsApplication : public Engine::BaseApplication
    {
    public:
        WindowsApplication() {}
        virtual ~WindowsApplication() {}

        void Initialize() override;
        void Shutdown() override;

        void Tick() override;

    private:
        void CreateMainWindow();
        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    protected:
        HWND m_hWnd;
        HDC  m_hDc;
    };
}