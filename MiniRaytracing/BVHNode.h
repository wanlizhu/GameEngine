#pragma once

#include "Object.h"

class BVHNode : public Object
{
public:
    BVHNode();
    BVHNode(const std::vector<std::shared_ptr<Object>>& objects,
            const INDEX_BOUNDS& span,    // [first, second)
            const DEPTH_BOUNDS& bounds); // [first, second)
    BVHNode(const BVHNode&) = delete;
    BVHNode& operator=(const BVHNode&) = delete;
    virtual ~BVHNode();

    virtual bool intersect(const Ray& ray,
                           const DEPTH_BOUNDS& bounds,
                           Intersection* hit) override;
    virtual AABB bounding_box(const DEPTH_BOUNDS& bounds) const override;

private:
    AABB _bounding_box;
    std::shared_ptr<Object> _left;
    std::shared_ptr<Object> _right;
};