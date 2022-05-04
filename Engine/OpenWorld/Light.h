#pragma once

#include "BasicTools.h"

class Light
{
public:
    using ARC = std::shared_ptr<Light>;
    
    Light() = default;
    Light(const Light&) = delete;
    Light& operator=(const Light&) = delete;
    
    void initWithDesc(const glm::vec3& pos);
    
    glm::vec3 const& position() const { return _position; }
    
private:
    glm::vec3 _position;
};
