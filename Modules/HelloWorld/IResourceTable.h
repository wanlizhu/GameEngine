#pragma once

#include "Utils.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "IDevice.h"

class IResourceTable
{
public:
    virtual Material* getMaterialWithName(const std::string& name) const = 0;
    virtual Mesh* getMeshWithName(const std::string& name) const = 0;
};
