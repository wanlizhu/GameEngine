#pragma once

#include "Material.h"
#include "Object.h"

class MaterialLambertian : public Material
{
public:
    MaterialLambertian(Texture* albedo);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;
    virtual bool emitted(const Ray& ray,
                         const Intersection& hit,
                         EmittedResult* result) override;

private:
    std::shared_ptr<Texture> _albedo;
};

std::shared_ptr<Material> make_lambertian(Texture* albedo);