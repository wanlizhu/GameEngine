#pragma once

#include "Config.h"
#include "Mesh.h"
#include "IDevice.h"

class IMeshLoader
{
public:
    using ARC = std::shared_ptr<IMeshLoader>;
    
    virtual ~IMeshLoader() = default;
    virtual std::vector<Mesh::ARC> load(const fs::path& path, IDevice* device) = 0;
};
