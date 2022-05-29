#pragma once

#include "RaytracingAPI.h"
#include "BasicTools.h"

struct Ray
{
    vec3 origin = vec3(0);
    vec3 direction = vec3(0);
    FLOAT time = 0.0;

    Ray() = default;
    Ray(const vec3& pos, const vec3& dir, FLOAT time_t);
    vec3 operator()(FLOAT t) const;
};