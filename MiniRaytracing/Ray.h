#pragma once

#include "RaytracingConfig.h"

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
    float time;

    Ray(const glm::vec3& pos = glm::vec3(NAN),
        const glm::vec3& dir = glm::vec3(NAN),
        float time_t = 0)
        : origin(pos)
        , direction(dir)
        , time(time_t)
    {}

    glm::vec3 operator()(float t) const { return origin + direction * t; }
    bool has_NaN() const { glm::any(glm::isnan(origin)) || glm::any(glm::isnan(direction)); }
};