#include "pch.h"
#include "Ray.h"

Ray::Ray(const vec3& pos,
         const vec3& dir,
         FLOAT time_t)
    : origin(pos)
    , direction(dir)
    , time(time_t)
{}

vec3 Ray::operator()(FLOAT t) const 
{ 
    return origin + direction * t;
}