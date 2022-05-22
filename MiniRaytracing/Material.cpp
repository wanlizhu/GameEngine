#include "MaterialMetal.h"
#include "MaterialLambertian.h"
#include "MaterialDielectric.h"

std::shared_ptr<Material> 
Material::deserialize(nlohmann::json json)
{
    std::string type = json["type"];
    std::shared_ptr<Material> material;

    if (type == "metal")
    {
        material = std::make_shared<MaterialMetal>(json_vec3(json["basecolor"]),
                                                   json["metallic"],
                                                   json["roughness"]);
    }
    else if (type == "lambertian")
    {
        material = std::make_shared<MaterialLambertian>(json_vec3(json["albedo"]));
    }
    else if (type == "dielectric")
    {
        material = std::make_shared<MaterialDielectric>(json_vec3(json["basecolor"]),
                                                        json["index_of_refraction"]);
    }

    assert(material);

    return material;
}