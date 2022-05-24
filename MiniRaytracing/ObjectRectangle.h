#pragma once

#include "Object.h"

class ObjectRectangle : public Object
{
public:
    ObjectRectangle();

    virtual bool intersect(const Ray& ray,
                           const DEPTH_BOUNDS& bounds,
                           Intersection* hit) override;
    virtual AABB bounding_box(const DEPTH_BOUNDS& bounds) const override;

private:

};

