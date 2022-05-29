#pragma once

#include "Object.h"

class ObjectPolygon : public Object
{
public:
    ObjectPolygon(const vec3* vertices,
                  int count,
                  const vec3& velocity,
                  Material* material,
                  const Transform& transform);

    virtual bool intersect(const Ray& ray,
                           const DEPTH_BOUNDS& bounds,
                           Intersection* hit) override;
    virtual AABB bounding_box(const DEPTH_BOUNDS& bounds) const override;

private:
    vec2 generate_uv(const vec3& pos) const;

private:
    std::shared_ptr<Material> _material;
    std::vector<Triangle> _triangles;
    vec3 _velocity;
};

std::shared_ptr<Object> make_polygon(const vec3* vertices,
                                     int count,
                                     const vec3& velocity,
                                     Material* material,
                                     const Transform& transform);

