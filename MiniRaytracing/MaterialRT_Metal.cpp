#include "MaterialRT_Metal.h"

MaterialRT_Metal::MaterialRT_Metal(const glm::vec3& basecolor,
                                   float metallic,
                                   float roughness)
    : _basecolor(basecolor)
    , _metallic(metallic)
    , _roughness(roughness)
{}

bool MaterialRT_Metal::scatter(const Ray& ray,
                               const Intersection& hit,
                               ScatteredResult* result)
{
    glm::vec3 reflected = glm::reflect(glm::normalize(ray.direction), hit.normal);
    glm::vec3 scattered = reflected + _roughness * random_in_unit_sphere();

    result->scatteredRays.push_back(Ray(hit.position, scattered));
    result->color = _basecolor;

    return glm::dot(scattered, hit.normal) > 0;
}