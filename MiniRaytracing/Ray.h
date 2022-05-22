#pragma once

#include "RaytracingAPI.h"
#include "BasicTools.h"

struct Ray
{
    vec3 origin = vec3(NAN);
    vec3 direction = vec3(NAN);
    FLOAT time = 0.0;

    Ray(const vec3& pos,
        const vec3& dir,
        FLOAT time_t)
        : origin(pos)
        , direction(dir)
        , time(time_t)
    {}

    vec3 operator()(FLOAT t) const { return origin + direction * t; }
    bool has_NaN() const { glm::any(glm::isnan(origin)) || glm::any(glm::isnan(direction)); }
};