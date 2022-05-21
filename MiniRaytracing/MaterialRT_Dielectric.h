#pragma once

#include "MaterialRT.h"
#include "ObjectRT.h"

class MaterialRT_Dielectric : public MaterialRT
{
public:
    MaterialRT_Dielectric(const glm::vec3& basecolor,
                          float ior);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;

private:
    glm::vec3 _basecolor;
    float _index_of_refraction;
};