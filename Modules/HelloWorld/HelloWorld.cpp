#include "HelloWorld.h"
#include "CDevice_Metal.h"
#include "CMeshLoader_OBJ.h"
#include "CImageLoader_STB.h"

void HelloWorld::initWithView(void* view)
{
    Int2 viewSize = drawableSizeWithMTKView(view);
    _searchPath.push_back(fs::current_path().string());
    
    std::string resourcePath = getEnv("ZHUJIE_RESOURCE_PATH");
    if (!resourcePath.empty())
    {
        std::vector<std::string> paths;
        split(resourcePath, ":;", &paths);
        
        _searchPath.insert(_searchPath.end(), paths.begin(), paths.end());
    }
    
    _device = std::make_shared<CDevice_Metal>(view);
    _camera = std::make_shared<Camera>();
    _camera->initWithPerspectiveProjection(_device.get(),
                                           glm::radians(45.0),
                                           (float)viewSize.x / viewSize.y,
                                           0.0,
                                           1.0);
    _framebuffer = nullptr;
    _meshLoader = std::make_shared<CMeshLoader_OBJ>();
    _imageLoader = std::make_shared<CImageLoader_STB>();
}

void HelloWorld::drawInView(void* view)
{
   if (!_device)
       return;
    
    _device->beginEncoding(CMD_Render);
    
    for (const auto& [name, material] : _materialMap)
    {
        _device->beginDebugLabel("");
        
        material->apply(_device.get(), this, _camera.get());
        
        for (const auto& meshName : material->meshNames())
        {
            Mesh* mesh = getMeshWithName(meshName);
            
            _device->beginDebugLabel("");
            mesh->draw(_device.get(), this);
            _device->endDebugLabel();
        }
        
        _device->endDebugLabel();
    }
    
    _device->endEncoding();
    _device->present(view);
    _device->commit();
}

void HelloWorld::resizeView(void* view)
{
    if (_camera)
    {
        Int2 size = drawableSizeWithMTKView(view);
        _camera->setAspect((float)size.x / size.y);
    }
}

void HelloWorld::zoomToFit(void* view)
{
    
}

void HelloWorld::load(const fs::path& path)
{}

Material* HelloWorld::getMaterialWithName(const std::string& name) const
{
    auto it = _materialMap.find(name);
    if (it == _materialMap.end())
        return nullptr;
    return it->second.get();
}

Mesh* HelloWorld::getMeshWithName(const std::string& name) const
{
    auto it = _meshMap.find(name);
    if (it == _meshMap.end())
        return nullptr;
    return it->second.get();
}
