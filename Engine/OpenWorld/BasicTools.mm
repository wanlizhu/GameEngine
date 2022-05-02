#include <Foundation/Foundation.h>
#include "BasicTools.h"

std::string currentPath()
{
    return [[[NSBundle mainBundle] bundlePath] cStringUsingEncoding:NSUTF8StringEncoding];
}

glm::ivec2 drawableSizeWithMTKView(void* _view)
{
    glm::ivec2 result;
    
    MTKView* view = (__bridge MTKView*)_view;
    result.x = view.drawableSize.width;
    result.y = view.drawableSize.height;
    
    return result;
}
