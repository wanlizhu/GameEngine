#pragma once

#include "RaytracingAPI.h"
#include "BasicTools.h"

class Texture : public std::enable_shared_from_this<Texture>
{
public:
    static std::shared_ptr<Texture> deserialize(nlohmann::json json);

    virtual ~Texture() = default;
    virtual RGBA32 sample(const vec2& uv, const vec3& pos) const = 0;
};