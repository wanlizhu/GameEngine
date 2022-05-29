#include "pch.h"
#include "Transform.h"

Transform Transform::deserialize(const nlohmann::json& json_transform)
{
    Transform transform;
    assert(false);
    return transform;
}

void Transform::set_scale(const vec3& scale)
{
    _scale = scale;
}

void Transform::set_rotation(const quat& rotate)
{
    _rotation = rotate;
}

void Transform::set_movement(const vec3& move)
{
    _movement = move;
}

bool Transform::is_identical() const
{
    return !_scale.has_value() && !_rotation.has_value() && !_movement.has_value();
}

vec3 Transform::operator()(const vec3& vec) const
{
    vec3 res = vec;

    if (_scale.has_value())
        res = res * _scale.value();

    if (_rotation.has_value())
        res = _rotation.value() * res;

    if (_movement.has_value())
        res = res + _movement.value();

    return res;
}

FLOAT Transform::operator()(const FLOAT& value) const
{
    FLOAT res = value;

    if (_scale.has_value())
        res = res * glm::length(_scale.value());

    return res;
}