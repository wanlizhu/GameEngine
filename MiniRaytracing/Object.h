#pragma once

#include "RaytracingAPI.h"
#include "Material.h"
#include "Ray.h"
#include "BasicTools.h"
#include "AABB.h"

struct Intersection
{
    vec3 position;
    vec3 normal;
    FLOAT t_hit;
    bool  front_face;
    Material* material;

    void setNormal(const Ray& ray, const vec3& norm);
};

class Object : public std::enable_shared_from_this<Object>
{
public:
    static std::shared_ptr<Object> deserialize(nlohmann::json json_obj,
                                               nlohmann::json json_mat);

    virtual ~Object() = default;
    virtual bool intersect(const Ray& ray, 
                           const DEPTH_BOUNDS& bounds,
                           Intersection* hit) = 0;
    virtual AABB bounding_box(const DEPTH_BOUNDS& bounds) const = 0;
};