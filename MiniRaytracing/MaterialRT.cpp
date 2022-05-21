#include "MaterialRT_Metal.h"
#include "MaterialRT_Lambertian.h"
#include "MaterialRT_Dielectric.h"

std::shared_ptr<MaterialRT> 
MaterialRT::deserialize(nlohmann::json json)
{
    std::string type = json["type"];
    std::shared_ptr<MaterialRT> material;

    if (type == "metal")
    {
        material = std::make_shared<MaterialRT_Metal>(json_vec3(json["basecolor"]),
                                                      json["metallic"],
                                                      json["roughness"]);
    }
    else if (type == "lambertian")
    {
        material = std::make_shared<MaterialRT_Lambertian>(json_vec3(json["albedo"]));
    }
    else if (type == "dielectric")
    {
        material = std::make_shared<MaterialRT_Dielectric>(json_vec3(json["basecolor"]),
                                                           json["index_of_refraction"]);
    }

    assert(material);

    return material;
}