#pragma once

#include "RaytracingAPI.h"
#include "Material.h"
#include "Ray.h"
#include "BasicTools.h"
#include "AABB.h"
#include "Transform.h"
#include "Triangle.h"

struct Intersection
{
    vec3 position = vec3(0);
    vec3 normal = vec3(0);
    vec2 uv = vec2(0);
    FLOAT depth_hit = FLT_MAX;
    bool front_face = true;
    Material* material = nullptr;

    void setNormal(const Ray& ray, const vec3& norm);
};

class Object : public std::enable_shared_from_this<Object>
{
public:
    static std::shared_ptr<Object> deserialize(const nlohmann::json& json_obj,
                                               const nlohmann::json& json_scene);

    virtual ~Object() = default;
    virtual bool intersect(const Ray& ray, 
                           const DEPTH_BOUNDS& bounds,
                           Intersection* hit) = 0;
    virtual AABB bounding_box(const DEPTH_BOUNDS& bounds) const = 0;
};