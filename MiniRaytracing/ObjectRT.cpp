#include "ObjectRT_Sphere.h"

void Intersection::setNormal(const Ray& ray,
                             const glm::vec3& norm)
{
    front_face = glm::dot(ray.direction, norm) < 0.0;
    normal = front_face ? norm : -norm;
}

std::shared_ptr<ObjectRT>
ObjectRT::deserialize(nlohmann::json json_obj,
                      nlohmann::json json_mat)
{
    std::string type = json_obj["type"];
    std::shared_ptr<MaterialRT> material;
    std::shared_ptr<ObjectRT> object;

    if (!json_mat.is_null())
    {
        material = MaterialRT::deserialize(json_mat);
    }

    if (type == "sphere")
    {
        assert(material);
        object = std::make_shared<ObjectRT_Sphere>(json_vec3(json_obj["center"]),
                                                   json_obj["radius"],
                                                   material.get());
    }

    assert(object);

    return object;
}