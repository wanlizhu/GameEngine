#pragma once

#include "RaytracingAPI.h"
#include "Ray.h"
#include "BasicTools.h"

class Camera
{
public:
    Camera(const vec3& pos = vec3(0, 0, 1),
           const vec3& target = vec3(0),
           const vec3& up = vec3(0, 1, 0),
           FLOAT aspect = 1.0,
           FLOAT fovy = 60,
           FLOAT aperture = 0,
           FLOAT focus = 10,
           FLOAT shutter_open = 0,
           FLOAT shutter_close = 0);

    void deserialize(nlohmann::json json);
    Ray generate_ray(FLOAT x, FLOAT y) const;

    FLOAT aspect() const { return _viewport.aspect; }

private:
    struct {
        vec3 o; // camera position
        vec3 u; // camera right unit vector
        vec3 v; // camera up unit vector
        vec3 w; // camera backward unit vector

        FLOAT aperture;
        FLOAT shutter_open;
        FLOAT shutter_close;
    } _camera;

    struct {
        vec3 origin;     // lower left corner 
        vec3 horizontal; // horizontal axis
        vec3 vertical;   // vertical axis
        FLOAT aspect;
    } _viewport;
};