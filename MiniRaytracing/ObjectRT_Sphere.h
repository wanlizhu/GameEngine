#pragma once

#include "ObjectRT.h"

class ObjectRT_Sphere : public ObjectRT
{
public:
    ObjectRT_Sphere(glm::vec3 center,
                    float radius,
                    MaterialRT* material);

    virtual bool intersect(const Ray& ray,
                           float minT,
                           float maxT,
                           Intersection* point) override;

private:
    std::shared_ptr<MaterialRT> _material;
    glm::vec3 _center;
    float _radius;
};