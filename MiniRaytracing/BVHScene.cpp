#include "BVHScene.h"
#include "ObjectSphere.h"
#include "MaterialLambertian.h"
#include "MaterialMetal.h"
#include "MaterialDielectric.h"

BVHScene::BVHScene()
{}

BVHScene::~BVHScene()
{
    release();
}

void BVHScene::deserialize(nlohmann::json json)
{
    for (const auto& json_obj : json["objects"])
    {
        if (json_obj["type"] == "random")
        {
            generate_random_objects(json_obj);
        }
        else
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

            _objects.push_back(Object::deserialize(json_obj, json_mat));
        }
    }

    _root = std::make_shared<BVHNode>(_objects, 
                                      INDEX_BOUNDS(0, (int)_objects.size()),
                                      DEPTH_BOUNDS(0, 1.0));
}

void BVHScene::generate_random_objects(nlohmann::json json_desc)
{
    std::vector<FLOAT> bounding_box = json_desc["bounding box"];
    std::vector<FLOAT> radius_range = json_desc["radius range"];
    int count = json_desc["count"];

    for (int i = 0; i < count; i++)
    {
        FLOAT choose_mat = random1();
        std::shared_ptr<Material> material;

        if (choose_mat < 0.8)
            material = std::make_shared<MaterialLambertian>(vec3(random1(), random1(), random1()));
        else if (choose_mat < 0.95)
            material = std::make_shared<MaterialMetal>(vec3(random1(), random1(), random1()), random1(), random1());
        else
            material = std::make_shared<MaterialDielectric>(vec3(random1(), random1(), random1()), random1());

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