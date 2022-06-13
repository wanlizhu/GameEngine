#include "pch.h"
#include "BVHScene.h"
#include "ObjectSphere.h"
#include "MaterialLambertian.h"
#include "MaterialMetal.h"
#include "MaterialDielectric.h"
#include "TextureSolidColor.h"
#include "TextureNoise.h"
#include "TextureChecker.h"
#include "TextureImage.h"

BVHScene::BVHScene()
{}

BVHScene::~BVHScene()
{
    release();
}

void BVHScene::deserialize(nlohmann::json json_scene)
{
    for (const auto& json_obj : json_scene["objects"])
    {
        if (json_obj["type"] == "random")
        {
            generate_random_objects(json_obj);
        }
        else
        {
            _objects.push_back(Object::deserialize(json_obj, json_scene));
        }
    }

    _root = std::make_shared<BVHNode>(_objects, 
                                      0, _objects.size(),
                                      DEPTH_BOUNDS(0, 1.0));
}

void BVHScene::generate_random_objects(nlohmann::json json_desc)
{
    std::vector<FLOAT> bounding_box = json_desc["bounding box"];
    std::vector<FLOAT> radius_range = json_desc["radius range"];
    int count = json_desc["count"];

    for (int i = 0; i < count; i++)
    {
        FLOAT choose_tex = random1();
        FLOAT choose_mat = random1();
        std::shared_ptr<Material> material;
        std::shared_ptr<Texture> texture;

        if (choose_tex < 0.8)
            texture = make_solid_color(random_color());
        else
            texture = make_checker(random_color(), vec4(1.0));

        if (choose_mat < 0.8)
            material = std::make_shared<MaterialLambertian>(texture.get());
        else if (choose_mat < 0.95)
            material = std::make_shared<MaterialMetal>(texture.get(), random1(), random1());
        else
            material = std::make_shared<MaterialDielectric>(texture.get(), random1());

        FLOAT x = random_in(bounding_box[0], bounding_box[3]);
        FLOAT y = random_in(bounding_box[1], bounding_box[4]);
        FLOAT z = random_in(bounding_box[2], bounding_box[5]);
        FLOAT radius = random_in(radius_range[0], radius_range[1]);
        auto object = std::make_shared<ObjectSphere>(vec3(x, y, z),
                                                     radius,
                                                     vec3(0),
                                                     material.get());
        _objects.push_back(object);
    }
}

void BVHScene::release()
{
    _objects.clear();
}

bool BVHScene::intersect(const Ray& ray,
                         const DEPTH_BOUNDS& bounds,
                         Intersection* hit)
{
    return _root->intersect(ray, bounds, hit);
}