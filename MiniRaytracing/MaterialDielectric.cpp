#include "MaterialDielectric.h"

MaterialDielectric::MaterialDielectric(Texture* basecolor,
                                       FLOAT ior)
    : _basecolor(basecolor->shared_from_this())
    , _index_of_refraction(ior)
{}

FLOAT reflectance(FLOAT cosine, FLOAT ior)
{
    // use Schlick's approximation for reflectance.
    auto r0 = (1 - ior) / (1 + ior);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool MaterialDielectric::scatter(const Ray& ray,
                                 const Intersection& hit,
                                 ScatteredResult* result)
{
    FLOAT ior = hit.front_face ? (1.0 / _index_of_refraction) : _index_of_refraction;
    vec3 ray_in = glm::normalize(ray.direction);
    FLOAT cos_theta = MIN(glm::dot(-ray_in, hit.normal), FLOAT(1.0));
    FLOAT sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = (ior * sin_theta) > 1.0;
    vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ior) > random1())
        direction = glm::reflect(ray_in, hit.normal);
    else
        direction = glm::refract(ray_in, hit.normal, ior);

    result->color = _basecolor->sample(hit.uv, hit.position);
    result->scattered_rays.push_back(Ray(hit.position, direction, ray.time));

    return true;
}

std::shared_ptr<Material> 
make_dielectric(Texture* basecolor, FLOAT ior)
{
    return std::make_shared<MaterialDielectric>(basecolor, ior);
}