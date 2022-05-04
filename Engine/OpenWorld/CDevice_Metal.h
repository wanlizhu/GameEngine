#pragma once

#include "IDevice.h"

class CDevice_Metal : public IDevice
{
public:
    struct Context;
    using ARC = std::shared_ptr<CDevice_Metal>;
    
    CDevice_Metal(void* view);
    virtual ~CDevice_Metal() = default;
    virtual IDeviceObject::ARC createModelWithDesc(const ModelDesc& desc) override;
    virtual BoundingBox boundingBox(void* object) override;
    virtual void release(void* object) override;

    virtual void beginEncoding() override;
    virtual void endEncoding() override;
    virtual void present(void* view) override;
    virtual void commit() override;
    
    virtual void drawModel(const IDeviceObject* model,
                           const Light* light,
                           const Camera* camera) override;
    
private:
    std::shared_ptr<Context> self;
};
