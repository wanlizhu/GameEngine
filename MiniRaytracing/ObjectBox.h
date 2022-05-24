#pragma once

#include "Object.h"

class ObjectBox : public Object
{
public:
    ObjectBox();

    virtual bool intersect(const Ray& ray,
                           const DEPTH_BOUNDS& bounds,
                           Intersection* hit) override;
    virtual AABB bounding_box(const DEPTH_BOUNDS& bounds) const override;

private:

};

