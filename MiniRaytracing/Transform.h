#pragma once

#include "BasicTools.h"

class Transform
{
public:
    static Transform deserialize(const nlohmann::json& json_transform);

    void set_scale(const vec3& scale);
    void set_rotation(const quat& rotate);
    void set_movement(const vec3& move);
    bool is_identical() const;

    vec3  operator()(const vec3& vec) const;
    FLOAT operator()(const FLOAT& value) const;

private:
    std::optional<vec3> _scale;
    std::optional<quat> _rotation;
    std::optional<vec3> _movement;
};

