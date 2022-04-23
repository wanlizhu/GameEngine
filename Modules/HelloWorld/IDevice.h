#pragma once

#include "Utils.h"

enum CommandType
{
    CMD_Render,
    CMD_Copy,
    CMD_Compute,
};

enum PixelFormat
{
    InvalidPixelFormat = 0,
    R32Uint = 53,          // MTLPixelFormatR32Uint
    R32Float = 55,         // MTLPixelFormatR32Float
    
    RG32Uint = 103,        // MTLPixelFormatRG32Uint
    RG32Float = 105,       // MTLPixelFormatRG32Float
    
    RGBA32Uint = 123,      // MTLPixelFormatRGBA32Uint
    RGBA32Float = 125,     // MTLPixelFormatRGBA32Float
    RGBA8Unorm = 70,       // MTLPixelFormatRGBA8Unorm
    
    Depth32Float = 252,    // MTLPixelFormatDepth32Float
    Stencil8Uint = 253,    // MTLPixelFormatStencil8
    Depth32Stencil8 = 260, // MTLPixelFormatDepth32Float_Stencil8
};

struct TextureDesc
{
    int width = 0;
    int height = 0;
    PixelFormat pixelFormat = InvalidPixelFormat;
    bool sharedStorageMode = false;
    const char* usage = ""; // "RWT" (Read/Write/renderTarget)
    const void* src = nullptr;
};

class  IDevice;
struct IDeviceObject
{
    using ARC = std::shared_ptr<IDeviceObject>;
    void* object = nullptr;
    IDevice* device = nullptr;
    
    IDeviceObject(const IDeviceObject&) = delete;
    IDeviceObject& operator=(const IDeviceObject&) = delete;
    IDeviceObject(void* obj, IDevice* dev)
        : object(obj), device(dev)
    {}
    virtual ~IDeviceObject();
    
    static ARC create(void* obj, IDevice* dev) {
        return std::make_shared<IDeviceObject>(obj, dev);
    }
};

// Vertex Array Object (shader program's input)
struct VAODesc
{
    IDeviceObject::ARC positions;
    IDeviceObject::ARC normals;
    IDeviceObject::ARC tangents;
    IDeviceObject::ARC texcoords;
};

struct AttachmentDescForFBO
{
    IDeviceObject::ARC texture;
    bool loadActionClear = true;
    bool storeActionDontCare = false;
    std::variant<glm::vec4, float, int> clearValue;
};

// Framebuffer Object (shader program's output)
struct FBODesc
{
    std::vector<AttachmentDescForFBO> colorAttachments;
    AttachmentDescForFBO depthAttachment;
    AttachmentDescForFBO stencilAttachment;
    
    std::optional<int> tileWidth;
    std::optional<int> tileHeight;
    std::optional<int> imageblockSampleLength; // tileDepth
};

// Shader Pogram Object (shader program and its resource bindings)
struct SPODesc
{
    std::string label;
    std::string shaderLibrary;
    std::string vertexFunc;
    std::string fragmentFunc;
    
    bool depthTestEnabled = true;
    bool depthWriteEnabled = true;
    bool stencilTestEnabled = false;
    bool stencilWriteEnabled = false;
    bool blendingEnabled = true;
    
    std::vector<PixelFormat> colorPixelFormats;
    PixelFormat depthPixelFormat = InvalidPixelFormat;
    PixelFormat stencilPixelFormat = InvalidPixelFormat;
    
    std::unordered_map<std::string, IDeviceObject::ARC> buffers;
    std::unordered_map<std::string, IDeviceObject::ARC> textures;
};

class IDevice
{
public:
    using ARC = std::shared_ptr<IDevice>;
    
    virtual ~IDevice() = default;
    virtual IDeviceObject::ARC createBufferWithBytes(const void* data, int size, bool sharedStorageMode, const char* label) = 0;
    virtual IDeviceObject::ARC createTextureWithDesc(const TextureDesc& desc, const char* label) = 0;
    virtual IDeviceObject::ARC createVAOWithDesc(const VAODesc& desc) = 0;
    virtual IDeviceObject::ARC createFBOWithDesc(const FBODesc& desc) = 0;
    virtual IDeviceObject::ARC createSPOWithDesc(const SPODesc& desc) = 0;
    virtual void release(void* object) = 0;
    virtual void replaceBufferRange(IDeviceObject::ARC buffer, int offset, int size, const void* src) = 0;
    virtual void replaceTextureRegion(IDeviceObject::ARC texture, Int2 offset, Int2 size, const void* src) = 0;
    virtual void readbackBufferRange(IDeviceObject::ARC buffer, int offset, int size, void* dst) = 0;
    virtual void readbackTextureRegion(IDeviceObject::ARC texture, Int2 offset, Int2 size, void* dst) = 0;
    
    virtual void beginEncoding(CommandType type) = 0;
    virtual void endEncoding() = 0;
    virtual void beginDebugLabel(const char* label) = 0;
    virtual void endDebugLabel() = 0;
    virtual void draw(IDeviceObject::ARC indexBuffer, int indexCount, int indexOffset, int indexTypeBits, int primitiveType) = 0;
    virtual void present(void* view) = 0;
    virtual void commit() = 0;
    virtual void setVAO(IDeviceObject::ARC vao) = 0;
    virtual void setFBO(IDeviceObject::ARC fbo) = 0;
    virtual void setSPO(IDeviceObject::ARC spo) = 0;
};

Int2 drawableSizeWithMTKView(void* view);
