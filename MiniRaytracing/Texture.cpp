#include "pch.h"
#include "Texture.h"
#include "TextureSolidColor.h"
#include "TextureChecker.h"
#include "TextureNoise.h"
#include "TextureImage.h"

std::shared_ptr<Texture> Texture::deserialize(const std::string& name,
                                              const nlohmann::json& json_scene)
{
    nlohmann::json json_tex;

    for (const auto& element : json_scene["textures"])
    {
        if (element["name"] == name)
        {
            json_tex = element;
            break;
        }
    }

    if (json_tex.is_null())
    {
        printf("Failed to find texture: %s\n", name.c_str());
        assert(false);
        return nullptr;
    }

    std::string type = json_tex["type"];
    std::shared_ptr<Texture> texture;

    if (type == "solid color")
    {
        texture = make_solid_color(json_vec4(json_tex["color"], 1.0));
    }
    else if (type == "checker")
    {
        std::shared_ptr<Texture> odd = deserialize(json_tex["odd"], json_scene);
        std::shared_ptr<Texture> even = deserialize(json_tex["even"], json_scene);
        texture = make_checker(odd.get(), even.get());
    }
    else if (type == "noise")
    {
        texture = make_noise(json_tex["style"], json_tex["scale"]);
    }
    else if (type == "image")
    {
        texture = make_image(json_tex["path"]);
    }

    assert(texture);
    return texture;
}