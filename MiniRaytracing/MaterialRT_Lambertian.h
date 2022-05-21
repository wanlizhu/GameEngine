#pragma once

#include "MaterialRT.h"
#include "ObjectRT.h"

class MaterialRT_Lambertian : public MaterialRT
{
public:
    MaterialRT_Lambertian(const glm::vec3& albedo);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;

private:
    glm::vec3 _albedo;
};