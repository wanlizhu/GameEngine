#pragma once

#include "Utils.h"
#include "IResourceTable.h"
#include "IMeshLoader.h"
#include "IImageLoader.h"

class HelloWorld : public IResourceTable
{
public:
    using ARC = std::shared_ptr<HelloWorld>;
    
    HelloWorld() = default;
    HelloWorld(const HelloWorld&) = delete;
    HelloWorld& operator=(const HelloWorld&) = delete;
    
    virtual ~HelloWorld() = default;
    virtual void initWithView(void* view);
    virtual void drawInView(void* view);
    virtual void resizeView(void* view);
    virtual void zoomToFit(void* view);
    virtual void load(const fs::path& path);
    
    virtual Material* getMaterialWithName(const std::string& name) const override;
    virtual Mesh* getMeshWithName(const std::string& name) const override;
    
private:
    std::unordered_map<std::string, Material::ARC> _materialMap;
    std::unordered_map<std::string, Mesh::ARC> _meshMap;
    std::vector<fs::path> _searchPath;
    std::string _defaultMaterialName = ".DEFAULT";
    
    Camera::ARC _camera;
    IDevice::ARC _device;
    IDeviceObject::ARC _framebuffer;
    IMeshLoader::ARC _meshLoader;
    IImageLoader::ARC _imageLoader;
};
