#pragma once

#include "BasicTools.h"
#include "Ray.h"
#include "AABB.h"

struct Intersection;

struct Triangle
{
    vec3 v1;
    vec3 v2;
    vec3 v3;

    Triangle() = default;
    Triangle(const vec3& a, const vec3& b, const vec3& c);

    bool intersect(const Ray& ray, 
                   const DEPTH_BOUNDS& bounds,
                   Intersection* hit) const;
    AABB bounding_box() const;
};

