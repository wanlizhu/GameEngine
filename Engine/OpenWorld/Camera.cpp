#include "Camera.h"

void Camera::initWithDesc(float fovy,
                          float aspect,
                          float znear,
                          float zfar)
{
    _fovy = fovy;
    _aspect = aspect;
    _znear = znear;
    _zfar = zfar;
    
    _projectionMatrix = glm::perspective(_fovy, _aspect, _znear, _zfar);
    _viewMatrix = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0), glm::vec3(0, 1, 0));
}

void Camera::keyboardEventDidReceive()
{}

void Camera::mouseEventDidReceive()
{}

void Camera::zoomToFit(BoundingBox const& box)
{
    glm::vec3 center = box.center();
    glm::vec3 extent = box.extent();
    float tan_theta = tan(_fovy * 0.5);
    float dist = (extent.y * 0.5) / tan_theta + extent.z * 0.5;
    
    _target = center;
    _pos = center + glm::vec3(0, 0, 1) * dist;
    _up = glm::vec3(0, 1, 0);
    _viewMatrix = glm::lookAt(_pos, _target, _up);
}

void Camera::setAspect(float aspect)
{
    _aspect = aspect;
    _projectionMatrix = glm::perspective(_fovy, _aspect, _znear, _zfar);
}
