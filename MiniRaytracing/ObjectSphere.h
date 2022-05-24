#pragma once

#include "Object.h"

class ObjectSphere : public Object
{
public:
    ObjectSphere(const vec3& center,
                 FLOAT radius,
                 const vec3& velocity,
                 Material* material);

    virtual bool intersect(const Ray& ray,
                           const DEPTH_BOUNDS& bounds,
                           Intersection* hit) override;
    virtual AABB bounding_box(const DEPTH_BOUNDS& bounds) const override;

private:
    vec2 generate_uv(const vec3& pos) const;

private:
    std::shared_ptr<Material> _material;
    vec3  _center;
    FLOAT _radius;
    vec3  _velocity;
};

std::shared_ptr<Object> make_sphere(const vec3& center,
                                    FLOAT radius,
                                    const vec3& velocity,
                                    Material* material);