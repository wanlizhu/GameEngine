#include "pch.h"
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

    hit->position = ray(root);
    hit->setNormal(ray, (hit->position - current_center) / _radius);
    hit->uv = generate_uv(hit->position);
    hit->depth_hit = root;
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

vec2 ObjectSphere::generate_uv(const vec3& pos) const
{
    FLOAT theta = acos(-pos.y);
    FLOAT phi = atan2(-pos.z, pos.x) + M_PI;
    FLOAT u = phi / (2 * M_PI);
    FLOAT v = theta / M_PI;

    return vec2(u, v);
}

std::shared_ptr<Object> make_sphere(const vec3& center,
                                    FLOAT radius,
                                    const vec3& velocity,
                                    Material* material)
{
    return std::make_shared<ObjectSphere>(center, radius, velocity, material);
}