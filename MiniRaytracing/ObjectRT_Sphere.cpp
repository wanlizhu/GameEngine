#include "ObjectRT_Sphere.h"

ObjectRT_Sphere::ObjectRT_Sphere(glm::vec3 center,
                                 float radius,
                                 MaterialRT* material)
    : _material(material->shared_from_this())
    , _center(center)
    , _radius(radius)
{}

bool ObjectRT_Sphere::intersect(const Ray& ray,
                                float t_min,
                                float t_max,
                                Intersection* hit)
{
    glm::vec3 oc = ray.origin - _center;
    float a = glm::dot(ray.direction, ray.direction);
    float b_half = glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - _radius * _radius;
    float discriminant = b_half * b_half - a * c;

    if (discriminant < 0)
        return false;

    float sqrtd = sqrt(discriminant);
    float root = (-b_half - sqrtd) / a;

    if (root < t_min || root > t_max)
    {
        root = (-b_half + sqrtd) / a;

        if (root < t_min || root > t_max)
            return false;
    }

    hit->t_hit = root;
    hit->position = ray(root);
    hit->setNormal(ray, (hit->position - _center) / _radius);
    hit->material = _material.get();

    return true;
}
