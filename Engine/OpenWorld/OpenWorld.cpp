#include "OpenWorld.h"
#include "CDevice_Metal.h"

void OpenWorld::initWithView(void* view)
{
    glm::ivec2 viewSize = drawableSizeWithMTKView(view);
    _device = std::make_shared<CDevice_Metal>(view);
    _camera = std::make_shared<Camera>();
    _camera->initWithDesc(glm::radians(45.0),
                          (float)viewSize.x / viewSize.y,
                          0.0,  // znear
                          1.0); // zfar
    _light = std::make_shared<Light>();
    _light->initWithDesc(glm::vec3(1000, 1000, 1000));
}

void OpenWorld::drawInView(void* view)
{
   if (!_device)
       return;
    
    _device->beginEncoding();
    
    for (const auto& model : _models)
    {
        _device->drawModel(model.get(), _light.get(), _camera.get());
    }
    
    _device->endEncoding();
    _device->present(view);
    _device->commit();
}

void OpenWorld::resizeView(void* view)
{
    if (_camera)
    {
        glm::ivec2 size = drawableSizeWithMTKView(view);
        _camera->setAspect((float)size.x / size.y);
    }
}

void OpenWorld::zoomToFit()
{
    assert(_boundingBox.isValid());
    _camera->zoomToFit(_boundingBox);
}

bool OpenWorld::loadModel(const std::string& name)
{
    std::string path = resourcePathWithName(name);
    ModelDesc modelDesc;
    
    if (path.empty() || !loadModelFile(path, &modelDesc))
    {
        printf("Failed to load model: %s\n", name.c_str());
        return false;
    }
    
    _models.push_back(_device->createModelWithDesc(modelDesc));
    _boundingBox.expand(_models.back()->boundingBox());
    zoomToFit();
    
    return true;
}
