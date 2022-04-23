#pragma once

#include "IDevice.h"

class CDevice_Metal : public IDevice
{
public:
    using ARC = std::shared_ptr<CDevice_Metal>;
    
    CDevice_Metal(void* view);
    virtual ~CDevice_Metal() = default;
    virtual IDeviceObject::ARC createBufferWithBytes(const void* data, int size, bool sharedStorageMode, const char* label) override;
    virtual IDeviceObject::ARC createTextureWithDesc(const TextureDesc& desc, const char* label) override;
    virtual IDeviceObject::ARC createVAOWithDesc(const VAODesc& desc) override;
    virtual IDeviceObject::ARC createFBOWithDesc(const FBODesc& desc) override;
    virtual IDeviceObject::ARC createSPOWithDesc(const SPODesc& desc) override;
    virtual void release(void* object) override;
    virtual void replaceBufferRange(IDeviceObject::ARC buffer, int offset, int size, const void* src) override;
    virtual void replaceTextureRegion(IDeviceObject::ARC texture, Int2 offset, Int2 size, const void* src) override;
    virtual void readbackBufferRange(IDeviceObject::ARC buffer, int offset, int size, void* dst) override;
    virtual void readbackTextureRegion(IDeviceObject::ARC texture, Int2 offset, Int2 size, void* dst) override;
    
    virtual void beginEncoding(CommandType type) override;
    virtual void endEncoding() override;
    virtual void beginDebugLabel(const char* label) override;
    virtual void endDebugLabel() override;
    virtual void draw(IDeviceObject::ARC indexBuffer, int indexCount, int indexOffset, int indexTypeBits, int primitiveType) override;
    virtual void present(void* view) override;
    virtual void commit() override;
    virtual void setVAO(IDeviceObject::ARC vao) override;
    virtual void setFBO(IDeviceObject::ARC fbo) override;
    virtual void setSPO(IDeviceObject::ARC spo) override;
    
private:
    struct PrivateData;
    std::shared_ptr<PrivateData> _data;
};
