#include "MaterialRT_Lambertian.h"

MaterialRT_Lambertian::MaterialRT_Lambertian(const glm::vec3& albedo)
    : _albedo(albedo)
{}

bool MaterialRT_Lambertian::scatter(const Ray& ray,
                                    const Intersection& hit,
                                    ScatteredResult* result)
{
    glm::vec3 scattered = hit.normal + random_on_unit_sphere();
    if (is_near_zero(scattered))
        scattered = hit.normal;

    result->scatteredRays.push_back(Ray(hit.position, scattered));
    result->color = _albedo;

    return glm::dot(result->scatteredRays[0].direction, hit.normal) > 0;
}