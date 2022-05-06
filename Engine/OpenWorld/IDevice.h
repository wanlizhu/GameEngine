#pragma once

#include "BasicTools.h"
#include "Camera.h"
#include "Light.h"
#include "ShaderConstants.h"

class IDevice;
class DeviceObject;

struct BufferDesc
{
    std::shared_ptr<std::vector<uint8_t>> data;
    DataType elementType = InvalidDataType;
    int elementCount = 0;
};

struct IndexBufferDesc : public BufferDesc
{
    PrimitiveType primitiveType = InvalidPrimitiveType;
    int indexTypeBites = 32; // 32 or 16
};

struct MaterialDesc
{
    std::string name;
    MaterialUniforms uniforms;
    std::unordered_map<int, std::string> textures;
};

struct MeshDesc
{
    std::string name;
    glm::mat4 transform;
    BoundingBox boundingBox;
    MaterialDesc material;
    
    std::unordered_map<int, BufferDesc> attributes;
    std::vector<IndexBufferDesc> indices;
};

struct ProgramDesc
{
    std::string name;
    std::string library;
    std::string vertexFunc;
    std::string fragmentFunc;
    
    std::unordered_map<std::string, float> constantsFloat;
    std::unordered_map<std::string, glm::vec4> constantsVec4;
    
    bool depthTestEnabled = true;
    bool depthWriteEnabled = true;
    bool stencilTestEnabled = false;
    bool stencilWriteEnabled = false;
    
    std::vector<PixelFormat> colorPixelFormats;
    std::vector<bool> blendingEnables;
    PixelFormat depthPixelFormat;
    PixelFormat stencilPixelFormat;
};

struct ModelDesc
{
    std::string name;
    BoundingBox boundingBox;
    std::vector<MeshDesc> meshes;
    ProgramDesc program;
};

class IDevice
{
public:
    using ARC = std::shared_ptr<IDevice>;
    
    virtual ~IDevice() = default;
    virtual std::shared_ptr<DeviceObject> createModelWithDesc(const ModelDesc& desc) = 0;
    virtual BoundingBox boundingBox(void* object) = 0;
    virtual void release(void* object) = 0;

    virtual void beginEncoding() = 0;
    virtual void endEncoding() = 0;
    virtual void present(void* view) = 0;
    virtual void commit() = 0;
    
    virtual void drawModel(const DeviceObject* model,
                           const Light* light,
                           const Camera* camera) = 0;
};

class DeviceFactory
{
public:
    static IDevice::ARC createDevice(void* view);
};

class DeviceObject
{
public:
    using ARC = std::shared_ptr<DeviceObject>;
    void* object = nullptr;
    IDevice* device = nullptr;
    
    static ARC create(void* obj, IDevice* dev) {
        return std::make_shared<DeviceObject>(obj, dev);
    }
    
    DeviceObject(const DeviceObject&) = delete;
    DeviceObject& operator=(const DeviceObject&) = delete;
    DeviceObject(void* obj, IDevice* dev)
    : object(obj), device(dev)
    {}
    virtual ~DeviceObject();
    
    BoundingBox boundingBox() const {
        return device->boundingBox(object);
    }
};
