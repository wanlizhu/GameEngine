#include "MaterialMetal.h"
#include "MaterialLambertian.h"
#include "MaterialDielectric.h"

std::shared_ptr<Material> 
Material::deserialize(const std::string& name,
                      const nlohmann::json& json_scene)
{
    nlohmann::json json_mat;
    
    for (const auto& element : json_scene["materials"])
    {
        if (element["name"] == name)
        {
            json_mat = element;
            break;
        }
    }

    std::string type = json_mat["type"];
    std::shared_ptr<Material> material;

    if (json_mat.is_null())
    {
        printf("Failed to find material: %s\n", name.c_str());
        assert(false);
        return nullptr;
    }

    if (type == "metal")
    {
        auto basecolor = Texture::deserialize(json_mat["basecolor"], json_scene);
        material = make_metal(basecolor.get(), json_mat["metallic"], json_mat["roughness"]);
    }
    else if (type == "lambertian")
    {
        auto albedo = Texture::deserialize(json_mat["albedo"], json_scene);
        material = make_lambertian(albedo.get());
    }
    else if (type == "dielectric")
    {
        auto basecolor = Texture::deserialize(json_mat["basecolor"], json_scene);
        material = make_dielectric(basecolor.get(), json_mat["index_of_refraction"]);
    }

    assert(material);
    return material;
}