#include "pch.h"
#include "ObjectBox.h"

std::shared_ptr<Object> make_rectangle(const vec3& v1,
                                       const vec3& v2,
                                       const vec3& v3,
                                       const vec3& v4,
                                       const vec3& velocity,
                                       Material* material,
                                       const Transform& transform)
{
    vec3 vertices[4] = {
        v1, v2, v3, v4
    };

    return make_polygon(vertices, 4, velocity, material, transform);
}

ObjectBox::ObjectBox(const vec3& center,
                     const vec3& extent,
                     const vec3& velocity,
                     Material* material,
                     const Transform& transform)
{
    vec3 min = center - extent * 0.5;
    vec3 max = center + extent * 0.5;
    vec3 v[8] = {
        min,
        min + vec3(extent.x, 0, 0),
        min + vec3(extent.xy, 0),
        min + vec3(0, extent.y, 0),
        max - vec3(extent.xy, 0),
        max - vec3(0, extent.y, 0),
        max,
        max - vec3(extent.x, 0, 0)
    };

    _faces.resize(6);
    _faces[0] = make_rectangle(v[0], v[1], v[2], v[3], velocity, material, transform); // front
    _faces[1] = make_rectangle(v[4], v[5], v[6], v[7], velocity, material, transform); // back
    _faces[2] = make_rectangle(v[3], v[2], v[6], v[7], velocity, material, transform); // top
    _faces[3] = make_rectangle(v[4], v[5], v[1], v[0], velocity, material, transform); // bottom
    _faces[4] = make_rectangle(v[1], v[5], v[6], v[2], velocity, material, transform); // right
    _faces[5] = make_rectangle(v[4], v[0], v[3], v[7], velocity, material, transform); // left
}

bool ObjectBox::intersect(const Ray& ray,
                          const DEPTH_BOUNDS& bounds,
                          Intersection* hit)
{
    Intersection tentative_hit;
    bool any_hit = false;

    for (int i = 0; i < (int)_faces.size(); i++)
    {
        if (_faces[i]->intersect(ray, bounds, hit) &&
            hit->depth_hit < tentative_hit.depth_hit)
        {
            tentative_hit = *hit;
            any_hit = true;
        }
    }

    if (any_hit)
    {
        *hit = tentative_hit;
    }

    return any_hit;
}

AABB ObjectBox::bounding_box(const DEPTH_BOUNDS& bounds) const
{
    AABB box = _faces[0]->bounding_box(bounds);

    for (int i = 1; i < _faces.size(); i++)
    {
        box = box + _faces[i]->bounding_box(bounds);
    }

    return box;
}

std::shared_ptr<Object> make_box(const vec3& center,
                                 const vec3& extent,
                                 const vec3& velocity,
                                 Material* material,
                                 const Transform& transform)
{
    return std::make_shared<ObjectBox>(center, extent, velocity, material, transform);
}