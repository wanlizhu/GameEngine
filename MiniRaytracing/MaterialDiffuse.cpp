#include "pch.h"
#include "MaterialDiffuse.h"

MaterialDiffuse::MaterialDiffuse(Texture* color)
    : _color(color->shared_from_this())
{}

bool MaterialDiffuse::scatter(const Ray& ray,
                              const Intersection& hit,
                              ScatteredResult* result)
{
    return false;
}

bool MaterialDiffuse::emitted(const Ray& ray,
                              const Intersection& hit,
                              EmittedResult* result)
{
    if (_color)
    {
        result->radiance = _color->sample(hit.uv, hit.position);
    }

    return _color != nullptr;
}

std::shared_ptr<Material> make_diffuse(Texture* color)
{
    return std::make_shared<MaterialDiffuse>(color);
}