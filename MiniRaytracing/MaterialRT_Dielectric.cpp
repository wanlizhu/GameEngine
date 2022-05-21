#include "MaterialRT_Dielectric.h"

MaterialRT_Dielectric::MaterialRT_Dielectric(const glm::vec3& basecolor,
                                             float ior)
    : _basecolor(basecolor)
    , _index_of_refraction(ior)
{}

float reflectance(float cosine, float ior)
{
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ior) / (1 + ior);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool MaterialRT_Dielectric::scatter(const Ray& ray,
                                    const Intersection& hit,
                                    ScatteredResult* result)
{
    float ior = hit.front_face ? (1.0 / _index_of_refraction) : _index_of_refraction;
    glm::vec3 ray_in = glm::normalize(ray.direction);
    float cos_theta = MIN(glm::dot(-ray_in, hit.normal), 1.0);
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    
    bool cannot_refract = (ior * sin_theta) > 1.0;
    glm::vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ior) > random1())
        direction = glm::reflect(ray_in, hit.normal);
    else
        direction = glm::refract(ray_in, hit.normal, ior);

    result->color = _basecolor;
    result->scatteredRays.push_back(Ray(hit.position, direction));

    return true;
}
