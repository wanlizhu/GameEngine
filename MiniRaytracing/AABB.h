#pragma once

#include "RaytracingAPI.h"
#include "BasicTools.h"
#include "Ray.h"

struct AABB
{
    vec3 min = vec3(NAN);
    vec3 max = vec3(NAN);

    AABB() {}
    AABB(const vec3& small,
         const vec3& big)
        : min(small)
        , max(big)
    {}

    bool intersect(const Ray& ray, const DEPTH_BOUNDS& bounds) const;
    bool is_valid() const;
};

AABB operator+(const AABB& a, const AABB& b);
