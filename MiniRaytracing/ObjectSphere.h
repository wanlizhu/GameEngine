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
                           FLOAT minT,
                           FLOAT maxT,
                           Intersection* point) override;

private:
    std::shared_ptr<Material> _material;
    vec3 _center;
    FLOAT _radius;
    vec3 _velocity;
};