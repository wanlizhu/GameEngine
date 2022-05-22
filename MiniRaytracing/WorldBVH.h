#pragma once

#include "RaytracingAPI.h"
#include "Object.h"

class WorldBVH
{
public:
    WorldBVH();
    WorldBVH(const WorldBVH&) = delete;
    WorldBVH& operator=(const WorldBVH&) = delete;
    virtual ~WorldBVH();

    void deserialize(nlohmann::json json);
    void release();
    bool intersect(const Ray& ray,
                   FLOAT t_min,
                   FLOAT t_max,
                   Intersection* hit);

private:
    void generate_random_objects(nlohmann::json json_desc);

private:
    std::vector<std::shared_ptr<Object>> _objects;
};