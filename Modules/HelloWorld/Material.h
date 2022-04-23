#pragma once

#include "Utils.h"
#include "IDevice.h"
#include "Camera.h"

struct MaterialDesc
{
    
};

class IResourceTable;

class Material
{
public:
    using ARC = std::shared_ptr<Material>;
    
    Material() = default;
    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;
    
    virtual ~Material() = default;
    virtual void initWithMaterialDesc(const MaterialDesc& desc, IDevice* device);
    virtual void apply(IDevice* device,
                       IResourceTable* resourceTable,
                       Camera* camera);
    
    virtual void attachMeshWithName(const std::string& name) { _meshNames.insert(name); }
    virtual std::unordered_set<std::string> const& meshNames() const { return _meshNames; }
    
private:
    std::string _name = "NoName";
    IDeviceObject::ARC _shaderProgram = nullptr;
    std::unordered_set<std::string> _meshNames;
};
