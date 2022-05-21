#pragma once

#include "RaytracingConfig.h"
#include "MaterialRT.h"
#include "Ray.h"
#include "BasicToolsRT.h"

struct Intersection
{
    glm::vec3 position;
    glm::vec3 normal;
    float t_hit;
    bool front_face;
    MaterialRT* material;

    void setNormal(const Ray& ray, const glm::vec3& norm);
};

class ObjectRT : public std::enable_shared_from_this<ObjectRT>
{
public:
    static std::shared_ptr<ObjectRT> deserialize(nlohmann::json json_obj,
                                                 nlohmann::json json_mat);

    virtual ~ObjectRT() = default;
    virtual bool intersect(const Ray& ray, 
                           float t_min,
                           float t_max,
                           Intersection* hit) = 0;
};