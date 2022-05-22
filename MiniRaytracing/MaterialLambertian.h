#pragma once

#include "Material.h"
#include "Object.h"

class MaterialLambertian : public Material
{
public:
    MaterialLambertian(const vec3& albedo);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;

private:
    vec3 _albedo;
};