#pragma once

#include "IDevice.h"

class CDevice_Vulkan : public IDevice
{
public:
    using ARC = std::shared_ptr<CDevice_Vulkan>;

    CDevice_Vulkan(void* view);
    virtual ~CDevice_Vulkan() = default;
    virtual DeviceObject::ARC createModelWithDesc(const ModelDesc & desc) override;
    virtual BoundingBox boundingBox(void* object) override;
    virtual void release(void* object) override;

    virtual void beginEncoding() override;
    virtual void endEncoding() override;
    virtual void present(void* view) override;
    virtual void commit() override;

    virtual void drawModel(const DeviceObject* model,
                           const Light* light,
                           const Camera* camera) override;

private:
    HWND _hwnd = NULL;
    
    std::unordered_map<void*, size_t> _types;
};

