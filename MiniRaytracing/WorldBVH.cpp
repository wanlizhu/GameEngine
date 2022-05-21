#include "WorldBVH.h"

WorldBVH::WorldBVH()
{}

WorldBVH::~WorldBVH()
{
    release();
}

void WorldBVH::deserialize(nlohmann::json json)
{
    for (const auto& json_obj : json["objects"])
    {
        std::string name_mat = json_obj["material"];
        nlohmann::json json_mat;

        for (const auto& element : json["materials"])
        {
            if (element["name"] == name_mat)
            {
                json_mat = element;
                break;
            }
        }

        _objects.push_back(ObjectRT::deserialize(json_obj, json_mat));
    }
}

void WorldBVH::release()
{
    _objects.clear();
}

bool WorldBVH::intersect(const Ray& ray,
                         float t_min,
                         float t_max,
                         Intersection* hit)
{
    bool hit_any = false;
    float closest_hit = t_max;
    Intersection temp;

    for (const auto& object : _objects)
    {
        if (object->intersect(ray, t_min, closest_hit, &temp))
        {
            hit_any = true;
            closest_hit = temp.t_hit;
            *hit = temp;
        }
    }

    return hit_any;
}