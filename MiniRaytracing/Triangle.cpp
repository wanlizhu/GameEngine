#include "pch.h"
#include "Triangle.h"
#include "Object.h"

Triangle::Triangle(const vec3& a, const vec3& b, const vec3& c)
    : v1(a), v2(b), v3(c)
{}

bool Triangle::intersect(const Ray& ray,
                         const DEPTH_BOUNDS& bounds, 
                         Intersection* hit) const
{
    assert(false);
    return false;
}

AABB Triangle::bounding_box() const
{
    assert(false);
    return {};
}
