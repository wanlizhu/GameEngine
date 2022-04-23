#include "Camera.h"

Camera::Camera()
    : _projMatrix(1.0)
    , _viewMatrix(1.0)
{}

void Camera::initWithPerspectiveProjection(IDevice* device,
                                           float fovy,
                                           float aspect,
                                           float znear,
                                           float zfar)
{
    _fovy = fovy;
    _aspect = aspect;
    _znear = znear;
    _zfar = zfar;
    _projMatrix = glm::perspective(_fovy, _aspect, _znear, _zfar);
    
    _buffer = device->createBufferWithBytes(nullptr,
                                            sizeof(Uniforms_Camera),
                                            true,
                                            "Camera");
    _uniforms.matrixViewProj = _projMatrix * _viewMatrix;
    _needsUpdateBuffer = true;
}

void Camera::bind(IDevice* device)
{}

void Camera::mouseEventDidReceive()
{}

void Camera::zoomToFit(BoundingBox const& box)
{}

void Camera::setAspect(float aspect)
{
    _aspect = aspect;
    _projMatrix = glm::perspective(_fovy, _aspect, _znear, _zfar);
    _uniforms.matrixViewProj = _projMatrix * _viewMatrix;
    _needsUpdateBuffer = true;
}

IDeviceObject* Camera::buffer()
{
    if (_needsUpdateBuffer)
    {
        _needsUpdateBuffer = false;
        _buffer->device->replaceBufferRange(_buffer, 0, sizeof(Uniforms_Camera), &_uniforms);
    }
    
    return _buffer.get();
}
