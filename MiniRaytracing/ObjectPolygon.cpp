#include "pch.h"
#include "ObjectPolygon.h"

ObjectPolygon::ObjectPolygon(const vec3* vertices,
                             int count,
                             const vec3& velocity,
                             Material* material,
                             const Transform& transform)
{
    assert(false);
}

bool ObjectPolygon::intersect(const Ray& ray,
                              const DEPTH_BOUNDS& bounds,
                              Intersection* hit)
{
    bool hit_any = false;
    
    for (int i = 0; i < _triangles.size(); i++)
    {
        if (_triangles[i].intersect(ray, bounds, hit))
        {
            hit->material = _material.get();
            hit->uv = generate_uv(hit->position);
            hit_any = true;
            break;
        }
    }

    return hit_any;
}

AABB ObjectPolygon::bounding_box(const DEPTH_BOUNDS& bounds) const
{
    AABB box = _triangles[0].bounding_box();

    for (int i = 1; i < _triangles.size(); i++)
    {
        box = box + _triangles[i].bounding_box();
    }

    return box;
}

vec2 ObjectPolygon::generate_uv(const vec3& pos) const
{
    assert(false);
    return {};
}

std::shared_ptr<Object> make_polygon(const vec3* vertices,
                                     int count,
                                     const vec3& velocity,
                                     Material* material,
                                     const Transform& transform)
{
    return std::make_shared<ObjectPolygon>(vertices, count, velocity, material, transform);
}

