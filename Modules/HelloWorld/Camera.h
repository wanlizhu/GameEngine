#pragma once

#include "Utils.h"
#include "IDevice.h"
#include "Shaders/ShaderTypes.h"

class Camera
{
public:
    using ARC = std::shared_ptr<Camera>;
    
    Camera();
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    
    virtual ~Camera() = default;
    virtual void initWithPerspectiveProjection(IDevice* device,
                                               float fovy,
                                               float aspect,
                                               float znear,
                                               float zfar);
    virtual void bind(IDevice* device);
    virtual void mouseEventDidReceive();
    virtual void zoomToFit(BoundingBox const& box);
    virtual void setAspect(float aspect);
    
    glm::mat4 const& viewMatrix() const { return _viewMatrix; }
    glm::mat4 const& projMatrix() const { return _projMatrix; }
    IDeviceObject* buffer();
    
private:
    IDeviceObject::ARC _buffer;
    Uniforms_Camera _uniforms;
    bool _needsUpdateBuffer = true;
    
    glm::mat4 _projMatrix;
    glm::mat4 _viewMatrix;
    
    float _fovy;
    float _aspect;
    float _znear;
    float _zfar;
};
