#include "pch.h"
#include "ObjectSphere.h"
#include "ObjectPolygon.h"
#include "ObjectBox.h"

void Intersection::setNormal(const Ray& ray, 
                             const vec3& norm)
{
    front_face = glm::dot(ray.direction, norm) < 0.0;
    normal = front_face ? norm : -norm;
}

std::shared_ptr<Object> 
Object::deserialize(const nlohmann::json& json_obj,
                    const nlohmann::json& json_scene)
{
    std::string type = json_obj["type"];
    Transform transform = Transform::deserialize(json_obj["transform"]);
    std::shared_ptr<Material> material = Material::deserialize(json_obj["material"], json_scene);
    std::shared_ptr<Object> object;

    if (type == "sphere")
    {
        object = make_sphere(json_vec3(json_obj["center"]),
                             json_obj["radius"],
                             json_vec3(json_obj["velocity"]),
                             material.get(),
                             transform);
    }
    else if (type == "polygon")
    {
        std::vector<vec3> vertices = json_vec3_array(json_obj["vertices"]);
        object = make_polygon(vertices.data(),
                              vertices.size(),
                              json_vec3(json_obj["velocity"]),
                              material.get(),
                              transform);
    }
    else if (type == "box")
    {
        object = make_box(json_vec3(json_obj["center"]),
                          json_vec3(json_obj["extent"]),
                          json_vec3(json_obj["velocity"]),
                          material.get(),
                          transform);
    }

    assert(object);
    return object;
}