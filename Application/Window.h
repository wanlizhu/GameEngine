#pragma once

#include <Windows.h>
#include "MiniRaytracing/RaytracingAPI.h"
#include "MiniVulkan/VulkanCanvas.h"

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
    MatrixMVP _mvp;
    ARC<VulkanCanvas> _canvas;
    std::vector<Model> _models;
};

