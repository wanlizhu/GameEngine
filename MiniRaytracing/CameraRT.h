#pragma once

#include "RaytracingConfig.h"
#include "Ray.h"
#include "BasicToolsRT.h"

class CameraRT
{
public:
    CameraRT(const glm::vec3& pos = glm::vec3(0, 0, 1),
             const glm::vec3& target = glm::vec3(0),
             const glm::vec3& up = glm::vec3(0, 1, 0),
             float aspect = 1.0,
             float fovy = 60,
             float aperture = 0,
             float focus = 10,
             float shutter_open = 0,
             float shutter_close = 0);

    void deserialize(nlohmann::json json);
    Ray generate_ray(float x, float y) const;

    float aspect() const { return _viewport.aspect; }

private:
    struct {
        glm::vec3 o; // camera position
        glm::vec3 u; // camera right unit vector
        glm::vec3 v; // camera up unit vector
        glm::vec3 w; // camera backward unit vector

        float aperture;
        float shutter_open;
        float shutter_close;
    } _camera;

    struct {
        glm::vec3 origin;     // lower left corner 
        glm::vec3 horizontal; // horizontal axis
        glm::vec3 vertical;   // vertical axis
        float aspect;
    } _viewport;
};