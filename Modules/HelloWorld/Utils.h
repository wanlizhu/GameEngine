#pragma once

#include "Config.h"

class BoundingBox
{
public:
    BoundingBox() : _min(0.0), _max(0.0) {}
    BoundingBox(glm::vec3 min, glm::vec3 max) : _min(min), _max(max) {}
    
    glm::vec3 const& min() const { return _min; }
    glm::vec3 const& max() const { return _max; }
    glm::vec3 center() const { return (_min + _max) * 0.5; }
    float radius() const { return glm::length(_max - _min) * 0.5; }
    void expand(const BoundingBox& box) { expand(box._min); expand(box._max); }
    void expand(const glm::vec3& point) {
        if (glm::all(glm::equal(_min, _max))) {
            _min = point;
            _max = point;
        } else {
            _min = glm::min(_min, point);
            _max = glm::max(_max, point);
        }
    }
    
private:
    glm::vec3 _min;
    glm::vec3 _max;
};

struct Blob
{
    const void* data = nullptr;
    size_t size = 0;
    
    Blob() = default;
    Blob(const void* data, size_t size)
        : data(data), size(size){}
    operator bool() const { return data != nullptr; }
};

std::string getEnv(const char* name);
void setEnv(const char* name, const char* value);
void split(const std::string& str, const char* seps, std::vector<std::string>* results);
std::string stringFormat(const char* format, ...);
