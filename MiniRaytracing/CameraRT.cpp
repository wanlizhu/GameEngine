#include "CameraRT.h"
#include "BasicToolsRT.h"

CameraRT::CameraRT(const glm::vec3& pos,
                   const glm::vec3& target,
                   const glm::vec3& up,
                   float aspect,
                   float fovy,
                   float aperture,
                   float focus,
                   float shutter_open,
                   float shutter_close)
{
    float viewport_h = 2.0 * tan(RADIANS(fovy) / 2.0);
    float viewport_w = viewport_h / aspect;

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
        (_viewport.horizontal / 2.0f) - 
        (_viewport.vertical / 2.0f) - 
        (focus * _camera.w);
}

void CameraRT::deserialize(nlohmann::json json)
{
    *this = CameraRT(json_vec3(json["position"]),
                     json_vec3(json["target"]),
                     json_vec3(json["up"]),
                     json["aspect"],
                     json["fovy"],
                     json["aperture"],
                     json["focus"],
                     json["shutter_open"],
                     json["shutter_close"]);
}

Ray CameraRT::generate_ray(float x, float y) const
{
    assert(x >= 0.0 && x <= 1.0);
    assert(y >= 0.0 && y <= 1.0);

    glm::vec2 jitter = _camera.aperture * 0.5f * random_in_unit_circle();
    glm::vec3 offset = _camera.u * jitter.x + _camera.v * jitter.y;

    glm::vec3 ray_begin = _camera.o + offset;
    glm::vec3 ray_end = _viewport.origin + (x * _viewport.horizontal) + (y * _viewport.vertical);
    float time = random_in(_camera.shutter_open, _camera.shutter_close);

    return Ray(ray_begin, 
               ray_end - ray_begin, 
               time);
}
