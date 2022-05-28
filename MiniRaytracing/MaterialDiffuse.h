#pragma once

#include "Material.h"
#include "Object.h"

class MaterialDiffuse : public Material
{
public:
    MaterialDiffuse(Texture* color);

    virtual bool scatter(const Ray& ray,
                         const Intersection& hit,
                         ScatteredResult* result) override;
    virtual bool emitted(const Ray& ray,
                         const Intersection& hit,
                         EmittedResult* result) override;

private:
    std::shared_ptr<Texture> _color;
};

std::shared_ptr<Material> make_diffuse(Texture* color);