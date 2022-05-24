#include "ObjectSphere.h"

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
    std::shared_ptr<Object> object;
    std::shared_ptr<Material> material;
   
    material = Material::deserialize(json_obj["material"],
                                     json_scene);

    if (type == "sphere")
    {
        object = make_sphere(json_vec3(json_obj["center"]),
                             json_obj["radius"],
                             json_vec3(json_obj["velocity"]),
                             material.get());
    }

    assert(object);
    return object;
}