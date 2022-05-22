#include "MaterialLambertian.h"

MaterialLambertian::MaterialLambertian(const vec3& albedo)
    : _albedo(albedo)
{}

bool MaterialLambertian::scatter(const Ray& ray,
                                 const Intersection& hit,
                                 ScatteredResult* result)
{
    vec3 scattered = hit.normal + random_on_unit_sphere();
    if (is_near_zero(scattered))
        scattered = hit.normal;

    result->scatteredRays.push_back(Ray(hit.position, scattered, ray.time));
    result->color = _albedo;

    return glm::dot(result->scatteredRays[0].direction, hit.normal) > 0;
}