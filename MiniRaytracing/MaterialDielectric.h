#pragma once

#include "Material.h"
#include "Object.h"

class MaterialDielectric : public Material
{
public:
    MaterialDielectric(const vec3& basecolor,
                       FLOAT ior);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;

private:
    vec3  _basecolor;
    FLOAT _index_of_refraction;
};