#pragma once

#include "RaytracingAPI.h"
#include "BasicTools.h"

class Texture : public std::enable_shared_from_this<Texture>
{
public:
    static std::shared_ptr<Texture> deserialize(const nlohmann::json& json_value,
                                                const nlohmann::json& json_scene);

    virtual ~Texture() = default;
    virtual vec4 sample(const vec2& uv, const vec3& pos) const = 0;
};