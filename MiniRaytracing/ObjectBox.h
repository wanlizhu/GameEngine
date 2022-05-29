#pragma once

#include "ObjectPolygon.h"

class ObjectBox : public Object
{
public:
    ObjectBox(const vec3& center,
              const vec3& extent,
              const vec3& velocity,
              Material* material,
              const Transform& transform);

    virtual bool intersect(const Ray& ray,
                           const DEPTH_BOUNDS& bounds,
                           Intersection* hit) override;
    virtual AABB bounding_box(const DEPTH_BOUNDS& bounds) const override;

private:
    std::vector<std::shared_ptr<Object>> _faces;
};

std::shared_ptr<Object> make_box(const vec3& center,
                                 const vec3& extent,
                                 const vec3& velocity,
                                 Material* material,
                                 const Transform& transform);
