#pragma once

#include "IMeshLoader.h"

class CMeshLoader_OBJ : public IMeshLoader
{
public:
    virtual ~CMeshLoader_OBJ() = default;
    virtual std::vector<Mesh::ARC> load(const fs::path& path, IDevice* device) override;
};
