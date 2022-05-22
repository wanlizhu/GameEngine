#pragma once

#include "Material.h"
#include "Object.h"

class MaterialMetal : public Material
{
public:
    MaterialMetal(const vec3& basecolor,
                  FLOAT metallic,
                  FLOAT roughness);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;

private:
    vec3  _basecolor;
    FLOAT _metallic;
    FLOAT _roughness;
};