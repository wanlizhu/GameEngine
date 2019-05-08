#include <memory>
#include <stdexcept>
#include <iostream>
#include <chrono>

#include "Global.h"
#include "IApplication.h"

using namespace Engine;

int main(int argc, char** argv)
{
    auto g_pApp = gpGlobal->GetApplication();
    if (!g_pApp)
        return 0;

    try
    {
        g_pApp->Initialize();
    }
    catch (const std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }

    while (!g_pApp->IsQuit()) {
        try
        {
            auto& fpsCounter = gpGlobal->GetFPSCounter();

            fpsCounter.BeginTick();
            g_pApp->Tick(fpsCounter.GetElapsedTime());
            fpsCounter.EndTick();
        }
        catch (const std::runtime_error& e)
        {
            std::cout << e.what() << std::endl;
            return -1;
        }
    }

    g_pApp->Shutdown();

    return 0;
}