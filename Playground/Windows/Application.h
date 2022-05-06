#pragma once

#include <Windows.h>
#include "OpenWorld/OpenWorld.h"

class Application
{
public:
    Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    virtual ~Application();

    int run();

private:
    HWND _hwnd = NULL;
    HDC  _dc = NULL;
    OpenWorld::ARC _world;
};

