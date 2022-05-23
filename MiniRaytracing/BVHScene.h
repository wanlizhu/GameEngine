#pragma once

#include "RaytracingAPI.h"
#include "Object.h"
#include "BVHNode.h"

class BVHScene
{
public:
    BVHScene();
    BVHScene(const BVHScene&) = delete;
    BVHScene& operator=(const BVHScene&) = delete;
    virtual ~BVHScene();

    void deserialize(nlohmann::json json);
    void release();
    bool intersect(const Ray& ray,
                   const DEPTH_BOUNDS& bounds,
                   Intersection* hit);

private:
    void generate_random_objects(nlohmann::json json_desc);

private:
    std::vector<std::shared_ptr<Object>> _objects;
    std::shared_ptr<BVHNode> _root;
};