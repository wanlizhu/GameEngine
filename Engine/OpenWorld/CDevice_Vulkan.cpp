#include "CDevice_Vulkan.h"
#include "BasicTools_Vulkan.h"

IDevice::ARC DeviceFactory::createDevice(void* view)
{
    return std::make_shared<CDevice_Vulkan>(view);
}

DeviceObject::~DeviceObject()
{
    if (device && object) {
        device->release(object);
    }

    device = nullptr;
    object = nullptr;
}

CDevice_Vulkan::CDevice_Vulkan(void* view)
{}

DeviceObject::ARC CDevice_Vulkan::createModelWithDesc(const ModelDesc& desc)
{
    return 0;
}

BoundingBox CDevice_Vulkan::boundingBox(void* object)
{
    size_t hash = _types.at(object);
    _types.erase(object);

    if (hash == typeid(Model).hash_code())
    {
        return ((Model*)object)->boundingBox;
    }
    else
    {
        assert(false);
        return {};
    }
}

void CDevice_Vulkan::release(void* object)
{}

void CDevice_Vulkan::beginEncoding()
{}

void CDevice_Vulkan::endEncoding()
{}

void CDevice_Vulkan::present(void* view)
{}

void CDevice_Vulkan::commit()
{}

void CDevice_Vulkan::drawModel(const DeviceObject* model,
                               const Light* light,
                               const Camera* camera)
{}
