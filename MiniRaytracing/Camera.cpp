#include "pch.h"
#include "Camera.h"
#include "BasicTools.h"

Camera::Camera(const vec3& pos,
               const vec3& target,
               const vec3& up,
               FLOAT aspect,
               FLOAT fovy,
               FLOAT aperture,
               FLOAT focus,
               FLOAT shutter_open,
               FLOAT shutter_close)
{
    FLOAT viewport_h = 2.0 * tan(RADIANS(fovy) / 2.0);
    FLOAT viewport_w = viewport_h / aspect;

    _camera.o = pos;
    _camera.w = glm::normalize(pos - target);
    _camera.u = glm::normalize(glm::cross(up, _camera.w));
    _camera.v = glm::cross(_camera.w, _camera.u);
    _camera.aperture = aperture;
    _camera.shutter_open = shutter_open;
    _camera.shutter_close = shutter_close;

    _viewport.aspect = aspect;
    _viewport.horizontal = focus * viewport_w * _camera.u;
    _viewport.vertical = focus * viewport_h * _camera.v;
    _viewport.origin = pos -
        (_viewport.horizontal / FLOAT(2.0)) -
        (_viewport.vertical / FLOAT(2.0)) -
        (focus * _camera.w);
}

void Camera::deserialize(nlohmann::json json)
{
    *this = Camera(json_vec3(json["position"]),
                   json_vec3(json["target"]),
                   json_vec3(json["up"]),
                   json["aspect"],
                   json["fovy"],
                   json["aperture"],
                   json["focus"],
                   json["shutter_open"],
                   json["shutter_close"]);
}

Ray Camera::generate_ray(FLOAT x, FLOAT y) const
{
    assert(x >= 0.0 && x <= 1.0);
    assert(y >= 0.0 && y <= 1.0);

    vec2 jitter = _camera.aperture * 0.5 * random_in_unit_circle();
    vec3 offset = _camera.u * jitter.x + _camera.v * jitter.y;

    vec3 ray_begin = _camera.o + offset;
    vec3 ray_end = _viewport.origin + (x * _viewport.horizontal) + (y * _viewport.vertical);
    FLOAT time = random_in(_camera.shutter_open, _camera.shutter_close);

    return Ray(ray_begin, 
               ray_end - ray_begin, 
               time);
}
