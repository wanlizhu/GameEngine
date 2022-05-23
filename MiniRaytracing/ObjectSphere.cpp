#include "ObjectSphere.h"

ObjectSphere::ObjectSphere(const vec3& center,
                           FLOAT radius,
                           const vec3& velocity,
                           Material* material)
    : _material(material->shared_from_this())
    , _center(center)
    , _radius(radius)
    , _velocity(velocity)
{}

bool ObjectSphere::intersect(const Ray& ray,
                             const DEPTH_BOUNDS& bounds,
                             Intersection* hit)
{
    vec3 current_center = _center + _velocity * ray.time;
    vec3 oc = ray.origin - current_center;
    FLOAT a = glm::dot(ray.direction, ray.direction);
    FLOAT b_half = glm::dot(oc, ray.direction);
    FLOAT c = glm::dot(oc, oc) - _radius * _radius;
    FLOAT discriminant = b_half * b_half - a * c;

    if (discriminant < 0)
        return false;

    FLOAT sqrtd = sqrt(discriminant);
    FLOAT root = (-b_half - sqrtd) / a;

    if (root < bounds.first || root > bounds.second)
    {
        root = (-b_half + sqrtd) / a;

        if (root < bounds.first || root > bounds.second)
            return false;
    }

    hit->t_hit = root;
    hit->position = ray(root);
    hit->setNormal(ray, (hit->position - current_center) / _radius);
    hit->material = _material.get();

    return true;
}

AABB ObjectSphere::bounding_box(const DEPTH_BOUNDS& bounds) const
{
    vec3 radius_extent(_radius);

    vec3 center_begin = _center + _velocity * bounds.first;
    AABB begin(center_begin - radius_extent, center_begin + radius_extent);

    vec3 center_end = _center + _velocity * bounds.second;
    AABB end(center_end - radius_extent, center_end + radius_extent);

    return begin + end;
}