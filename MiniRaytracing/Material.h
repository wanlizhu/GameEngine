#pragma once

#include "RaytracingAPI.h"
#include "BasicTools.h"
#include "Ray.h"

struct Intersection;

struct ScatteredResult
{
    vec3 color;
    std::vector<Ray> scatteredRays;
};

class Material : public std::enable_shared_from_this<Material>
{
public:
    static std::shared_ptr<Material> deserialize(nlohmann::json json);

    virtual ~Material() = default;
    virtual bool scatter(const Ray& ray, 
                         const Intersection& hit,
                         ScatteredResult* result) = 0;
};