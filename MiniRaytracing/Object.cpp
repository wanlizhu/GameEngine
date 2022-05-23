#include "ObjectSphere.h"

void Intersection::setNormal(const Ray& ray, 
                             const vec3& norm)
{
    front_face = glm::dot(ray.direction, norm) < 0.0;
    normal = front_face ? norm : -norm;
}

std::shared_ptr<Object>
Object::deserialize(nlohmann::json json_obj,
                    nlohmann::json json_mat)
{
    std::string type = json_obj["type"];
    std::shared_ptr<Material> material;
    std::shared_ptr<Object> object;

    if (!json_mat.is_null())
    {
        material = Material::deserialize(json_mat);
    }

    if (type == "sphere")
    {
        assert(material);
        object = std::make_shared<ObjectSphere>(json_vec3(json_obj["center"]),
                                                json_obj["radius"],
                                                json_vec3(json_obj["velocity"]),
                                                material.get());
    }

    assert(object);

    return object;
}