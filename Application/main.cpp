#include "Window.h"

int main(int argc, char** argv)
{
#if 1
    RaytracingCreateInfo info;
    //info.sceneFile = "spheres.json";
    info.sceneFile = "spheres - luminous floor.json";
    auto result = dispatch_raytracing(info);
    result->wait();

    return 0;
#else
    Window win;
    return win.run();
#endif
}