#include "pch.h"
#include "MaterialMetal.h"

MaterialMetal::MaterialMetal(Texture* basecolor,
                             FLOAT metallic,
                             FLOAT roughness)
    : _basecolor(basecolor->shared_from_this())
    , _metallic(metallic)
    , _roughness(roughness)
{}

bool MaterialMetal::scatter(const Ray& ray,
                            const Intersection& hit,
                            ScatteredResult* result)
{
    vec3 reflected = glm::reflect(glm::normalize(ray.direction), hit.normal);
    vec3 scattered = reflected + _roughness * random_in_unit_sphere();

    result->radiance = _basecolor->sample(hit.uv, hit.position);
    result->new_rays.push_back(Ray(hit.position, scattered, ray.time));

    return glm::dot(scattered, hit.normal) > 0;
}

bool MaterialMetal::emitted(const Ray& ray,
                            const Intersection& hit,
                            EmittedResult* result)
{
    return false;
}

std::shared_ptr<Material> make_metal(Texture* basecolor,
                                     FLOAT metallic,
                                     FLOAT roughness)
{
    return std::make_shared<MaterialMetal>(basecolor, metallic, roughness);
}