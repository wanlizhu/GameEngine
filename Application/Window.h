#pragma once

#include <Windows.h>
#include "VulkanCanvas.h"

class Window
{
public:
    Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    virtual ~Window();

    int run();

private:
    HWND _hwnd = NULL;
    HDC  _hdc = NULL;
    ARC<VulkanCanvas> _canvas;
};

