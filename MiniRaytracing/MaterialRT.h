#pragma once

#include "RaytracingConfig.h"
#include "BasicToolsRT.h"
#include "Ray.h"

struct Intersection;

struct ScatteredResult
{
    glm::vec3 color;
    std::vector<Ray> scatteredRays;
};

class MaterialRT : public std::enable_shared_from_this<MaterialRT>
{
public:
    static std::shared_ptr<MaterialRT> deserialize(nlohmann::json json);

    virtual ~MaterialRT() = default;
    virtual bool scatter(const Ray& ray, 
                         const Intersection& hit,
                         ScatteredResult* result) = 0;
};