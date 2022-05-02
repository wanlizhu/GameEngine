#pragma once
#include "CDevice_Metal.h"

struct Material
{
    NSString* name = nil;
    bool enabled = false;
};

struct IndexDraw
{
    id<MTLBuffer> indexBuffer;
    MTLPrimitiveType primitiveType;
    MTLIndexType indexType;
    int indexCount = 0;
    int indexOffset = 0;
};

struct Mesh
{
    NSString* name = nil;
    std::unordered_map<int, id<MTLBuffer>> ubos_VS;
    BoundingBox boundingBox;
    Material material;
    
    std::unordered_map<int, id<MTLBuffer>> attributes;
    std::vector<IndexDraw> indices;
};

struct Program
{
    NSString* name = nil;
    id<MTLFunction> vertexFunc;
    id<MTLFunction> fragmentFunc;
    MTLRenderPipelineReflection* reflection = nullptr;
    id<MTLRenderPipelineState> renderPipelineState;
    id<MTLDepthStencilState> depthStencilState;
};

struct Model
{
    NSString* name = nil;
    BoundingBox boundingBox;
    std::vector<Mesh> meshes;
    Program program;
};

id<MTLBuffer> createBufferWithBytes(id<MTLCommandQueue> commandQueue, const void* data, size_t size, bool sharedStorageMode, const char* label);
id<MTLTexture> createTextureWithPath(id<MTLCommandQueue> commandQueue, const char* path, const char* label);
Program createProgramWithDesc(id<MTLDevice> device, const ProgramDesc& desc);
Material createMaterialWithDesc(id<MTLDevice> device, const MaterialDesc& desc);

MTLPrimitiveType getPrimitiveType(const char* type);
MTLStencilDescriptor* getStencilDesc(bool stencilTest, bool stencilWrite);
MTLRenderPipelineColorAttachmentDescriptor* getBlendingDesc(bool blendingEnabled);
int getResourceIndex(MTLRenderPipelineReflection* reflection, std::string name, std::string stage);
MTLVertexFormat getVertexFormat(MTLDataType type);
int getDataTypeLength(MTLDataType type);
MTLVertexDescriptor* getVertexDesc(id<MTLFunction> vertexFunc, bool combinedLayout);
int getAttributeIndex(const std::string& name, id<MTLFunction> vertexFunc);
MTLPrimitiveType getPrimitiveType(PrimitiveType type);
