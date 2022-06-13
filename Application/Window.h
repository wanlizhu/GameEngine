#pragma once

#include <Windows.h>
#include "MiniRaytracing/RaytracingAPI.h"
//#include "MiniVulkan/VulkanAPI.h"

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
    //MatrixMVP _mvp;
};

