#pragma once

#include "RaytracingConfig.h"
#include "ObjectRT.h"

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
                   float t_min,
                   float t_max,
                   Intersection* hit);

private:
    std::vector<std::shared_ptr<ObjectRT>> _objects;
};