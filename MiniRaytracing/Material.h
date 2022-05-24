#pragma once

#include "RaytracingAPI.h"
#include "BasicTools.h"
#include "Ray.h"
#include "Texture.h"

struct Intersection;

struct ScatteredResult
{
    vec4 color;
    std::vector<Ray> scattered_rays;
};

class Material : public std::enable_shared_from_this<Material>
{
public:
    static std::shared_ptr<Material> deserialize(const std::string& name, 
                                                 const nlohmann::json& json_scene);

    virtual ~Material() = default;
    virtual bool scatter(const Ray& ray, 
                         const Intersection& hit,
                         ScatteredResult* result) = 0;
};