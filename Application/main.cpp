#include "Window.h"

int main(int argc, char** argv)
{
#if 1
    RaytracingCreateInfo info;
    info.sceneFile = "spheres.json";
    auto result = dispatchRaytracing(info);
    result->wait();
    system("pause");

    return 0;
#else
    Window win;
    return win.run();
#endif
}