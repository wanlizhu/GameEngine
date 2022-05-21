#pragma once

#include "MaterialRT.h"
#include "ObjectRT.h"

class MaterialRT_Metal : public MaterialRT
{
public:
    MaterialRT_Metal(const glm::vec3& basecolor,
                     float metallic,
                     float roughness);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;

private:
    glm::vec3 _basecolor;
    float _metallic;
    float _roughness;
};