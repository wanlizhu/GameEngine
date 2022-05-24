#pragma once

#include "Material.h"
#include "Object.h"

class MaterialDielectric : public Material
{
public:
    MaterialDielectric(Texture* basecolor,
                       FLOAT ior);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;

private:
    std::shared_ptr<Texture>  _basecolor;
    FLOAT _index_of_refraction;
};

std::shared_ptr<Material> make_dielectric(Texture* basecolor, FLOAT ior);