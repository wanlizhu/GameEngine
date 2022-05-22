#include "MaterialMetal.h"

MaterialMetal::MaterialMetal(const vec3& basecolor,
                             FLOAT metallic,
                             FLOAT roughness)
    : _basecolor(basecolor)
    , _metallic(metallic)
    , _roughness(roughness)
{}

bool MaterialMetal::scatter(const Ray& ray,
                            const Intersection& hit,
                            ScatteredResult* result)
{
    vec3 reflected = glm::reflect(glm::normalize(ray.direction), hit.normal);
    vec3 scattered = reflected + _roughness * random_in_unit_sphere();

    result->scatteredRays.push_back(Ray(hit.position, scattered, ray.time));
    result->color = _basecolor;

    return glm::dot(scattered, hit.normal) > 0;
}