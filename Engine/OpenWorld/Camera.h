#pragma once

#include "BasicTools.h"

class Camera
{
public:
    using ARC = std::shared_ptr<Camera>;
    
    Camera() = default;
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    
    void initWithPerspectiveProjection(float fovy,
                                       float aspect,
                                       float znear,
                                       float zfar);
    void keyboardEventDidReceive();
    void mouseEventDidReceive();
    void zoomToFit(BoundingBox const& box);
    void setAspect(float aspect);
    glm::mat4 const& viewMatrix() const { return _viewMatrix; }
    glm::mat4 const& projectionMatrix() const { return _projectionMatrix; }
    
private:
    glm::mat4 _viewMatrix;
    glm::mat4 _projectionMatrix;
    
    float _fovy;
    float _aspect;
    float _znear;
    float _zfar;
    
    glm::vec3 _pos;
    glm::vec3 _target;
    glm::vec3 _up;
};
