#pragma once

#include "BasicTools.h"
#include "IDevice.h"
#include "Camera.h"

class OpenWorld
{
public:
    using ARC = std::shared_ptr<OpenWorld>;
    
    OpenWorld() = default;
    OpenWorld(const OpenWorld&) = delete;
    OpenWorld& operator=(const OpenWorld&) = delete;
    
    void initWithView(void* view);
    void drawInView(void* view);
    void resizeView(void* view);
    void messageDidReceive(int message, MessageParameter param);
    void zoomToFit();
    bool loadModel(const std::string& name);
    
private:
    IDevice::ARC _device;
    Camera::ARC _camera;
    Light::ARC _light;
    BoundingBox _boundingBox;
    std::vector<DeviceObject::ARC> _models;
};
