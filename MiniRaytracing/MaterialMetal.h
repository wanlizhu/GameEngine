#pragma once

#include "Material.h"
#include "Object.h"

class MaterialMetal : public Material
{
public:
    MaterialMetal(Texture* basecolor,
                  FLOAT metallic,
                  FLOAT roughness);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;
    virtual bool emitted(const Ray& ray,
                         const Intersection& hit,
                         EmittedResult* result) override;

private:
    std::shared_ptr<Texture>  _basecolor;
    FLOAT _metallic;
    FLOAT _roughness;
};

std::shared_ptr<Material> make_metal(Texture* basecolor, 
                                     FLOAT metallic,
                                     FLOAT roughness);