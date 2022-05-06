#include "MetalTools.h"

id<MTLBuffer> createBufferWithBytes(id<MTLCommandQueue> commandQueue,
                                    const void* data,
                                    size_t size,
                                    bool sharedStorageMode,
                                    const char* label)
{
    if (size == 0) {
        return nullptr;
    }
    
    id<MTLBuffer> buffer = nil;
    
    if (data)
    {
        if (sharedStorageMode)
        {
            buffer = [commandQueue.device newBufferWithBytes:data
                                                      length:size
                                                     options:MTLResourceStorageModeShared];
        }
        else
        {
            id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
            id<MTLBlitCommandEncoder> copyEncoder = [commandBuffer blitCommandEncoder];
            id<MTLBuffer> staging = [commandQueue.device newBufferWithBytes:data
                                                                     length:size
                                                                    options:MTLResourceStorageModeShared];
            buffer = [commandQueue.device newBufferWithLength:size
                                                      options:MTLResourceStorageModePrivate];
            [copyEncoder copyFromBuffer:staging
                           sourceOffset:0
                               toBuffer:buffer
                      destinationOffset:0
                                   size:size];
            [copyEncoder endEncoding];
            [commandBuffer commit];
            [commandBuffer waitUntilCompleted];
        }
    }
    else
    {
        MTLResourceOptions options = sharedStorageMode ?
        MTLResourceStorageModeShared :
        MTLResourceStorageModePrivate;
        
        buffer = [commandQueue.device newBufferWithLength:size
                                                  options:options];
    }
    
    if (label) [buffer setLabel:[NSString stringWithUTF8String:label]];
    
    return buffer;
}

void replaceTextureRegion(id<MTLCommandQueue> commandQueue,
                          id<MTLTexture> texture,
                          glm::ivec2 offset,
                          glm::ivec2 size,
                          const void* src)
{
    if (size.x == 0 || size.y == 0)
        size = glm::ivec2(texture.width, texture.height);
    
    id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
    id<MTLBlitCommandEncoder> copyEncoder = [commandBuffer blitCommandEncoder];
    id<MTLBuffer> staging = [commandQueue.device newBufferWithBytes:src
                                                             length:size.x * size.y * sizeof(int)
                                                            options:MTLResourceStorageModeShared];
    
    [copyEncoder copyFromBuffer:staging
                   sourceOffset:0
              sourceBytesPerRow:size.x * sizeof(int)
            sourceBytesPerImage:size.x * size.y * sizeof(int)
                     sourceSize:MTLSizeMake(size.x, size.y, 1)
                      toTexture:texture
               destinationSlice:0
               destinationLevel:0
              destinationOrigin:MTLOriginMake(0, 0, 0)];
    [copyEncoder endEncoding];
    
    if (commandBuffer)
    {
        [commandBuffer commit];
        [commandBuffer waitUntilCompleted];
    }
}

id<MTLTexture> createTextureWithPath(id<MTLCommandQueue> commandQueue,
                                     const char* path,
                                     const char* label)
{
    std::vector<uint32_t> pixels;
    int width = 0;
    int height = 0;
    
    if (!loadImageFile(path, &pixels, &width, &height)) {
        return nullptr;
    }
    
    MTLTextureDescriptor* descriptor = [[MTLTextureDescriptor alloc] init];
    descriptor.textureType = MTLTextureType2D;
    descriptor.pixelFormat = MTLPixelFormatRGBA8Unorm;
    descriptor.width = width;
    descriptor.height = height;
    descriptor.depth = 1;
    descriptor.mipmapLevelCount = 1;
    descriptor.sampleCount = 1;
    descriptor.arrayLength = 1;
    descriptor.storageMode = MTLStorageModePrivate;
    descriptor.usage = MTLTextureUsageShaderRead;
    
    id<MTLTexture> texture = [commandQueue.device newTextureWithDescriptor:descriptor];
    if (label) [texture setLabel:[NSString stringWithUTF8String:label]];
    
    replaceTextureRegion(commandQueue, texture, glm::ivec2(0), glm::ivec2(0), pixels.data());
    
    return texture;
}

MTLPrimitiveType getPrimitiveType(const char* type)
{
    if (strcmp(type, "P") == 0) return MTLPrimitiveTypePoint;
    if (strcmp(type, "L") == 0) return MTLPrimitiveTypeLine;
    if (strcmp(type, "T") == 0) return MTLPrimitiveTypeTriangle;
    return MTLPrimitiveTypeTriangle;
}

MTLStencilDescriptor* getStencilDesc(bool stencilTest, bool stencilWrite)
{
    MTLStencilDescriptor* desc = [[MTLStencilDescriptor alloc] init];
    desc.stencilCompareFunction = MTLCompareFunctionAlways;
    desc.stencilFailureOperation = MTLStencilOperationKeep;
    desc.depthFailureOperation = MTLStencilOperationKeep;
    desc.depthStencilPassOperation = MTLStencilOperationKeep;
    desc.readMask = UINT32_MAX;
    desc.writeMask = UINT32_MAX;
    
    if (stencilTest)
    {
        desc.stencilCompareFunction = MTLCompareFunctionEqual;
        desc.readMask = UINT32_MAX;
        desc.writeMask = 0;
    }
    
    if (stencilWrite)
    {
        desc.depthStencilPassOperation = MTLStencilOperationReplace;
        desc.readMask = UINT32_MAX;
        desc.writeMask = UINT32_MAX;
    }
    
    return desc;
}

MTLRenderPipelineColorAttachmentDescriptor* getBlendingDesc(bool blendingEnabled)
{
    MTLRenderPipelineColorAttachmentDescriptor* desc = [[MTLRenderPipelineColorAttachmentDescriptor alloc] init];
    desc.pixelFormat = MTLPixelFormatInvalid;
    desc.blendingEnabled = blendingEnabled ? YES : NO;
    desc.writeMask = MTLColorWriteMaskAll;
    
    if (blendingEnabled)
    {
        desc.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        desc.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        desc.rgbBlendOperation = MTLBlendOperationAdd;
        desc.sourceAlphaBlendFactor = MTLBlendFactorOne;
        desc.destinationAlphaBlendFactor = MTLBlendFactorZero;
        desc.alphaBlendOperation = MTLBlendOperationAdd;
    }
    
    return desc;
}

int getResourceIndex(MTLRenderPipelineReflection* reflection,
                     std::string name,
                     std::string stage)
{
    static std::unordered_map<void*,
                              std::unordered_map<std::string,
                                                 std::unordered_map<std::string,
                                                                    int>>> cache;
    void* reflection_c = (__bridge void*)reflection;
    if (cache.find(reflection_c) != cache.end() &&
        cache.at(reflection_c).find(stage) != cache.at(reflection_c).end() &&
        cache.at(reflection_c).at(stage).find(name) != cache.at(reflection_c).at(stage).end())
    {
        return cache.at(reflection_c).at(stage).at(name);
    }
    
    NSArray<MTLArgument*>* argumentList = (stage == "VS" ? reflection.vertexArguments :
                                           reflection.fragmentArguments);
    int index = -1;
    
    for (MTLArgument* argument in argumentList)
    {
        if ([argument.name isEqualToString:[NSString stringWithUTF8String:name.c_str()]])
        {
            index = (int)argument.index;
            break;
        }
    }
    
    cache[reflection_c][stage][name] = index;
    
    return index;
}

MTLVertexFormat getVertexFormat(MTLDataType type)
{
    switch (type)
    {
        case MTLDataTypeFloat:  return MTLVertexFormatFloat;
        case MTLDataTypeFloat2: return MTLVertexFormatFloat2;
        case MTLDataTypeFloat3: return MTLVertexFormatFloat3;
        case MTLDataTypeFloat4: return MTLVertexFormatFloat4;
        default: assert(false); return MTLVertexFormatInvalid;
    }
}

int getDataTypeLength(MTLDataType type)
{
    switch (type)
    {
        case MTLDataTypeFloat:  return sizeof(float);
        case MTLDataTypeFloat2: return sizeof(float) * 2;
        case MTLDataTypeFloat3: return sizeof(float) * 3;
        case MTLDataTypeFloat4: return sizeof(float) * 4;
        default: assert(false); return 0;
    }
}

MTLVertexDescriptor* getVertexDesc(id<MTLFunction> vertexFunc, bool combinedLayout)
{
    MTLVertexDescriptor* desc = [[MTLVertexDescriptor alloc] init];
    
    if (combinedLayout)
    {
        int offset = 0;
        
        for (int i = 0; i < vertexFunc.vertexAttributes.count; i++)
        {
            auto attribute = vertexFunc.vertexAttributes[i];
            desc.attributes[i].format = getVertexFormat(attribute.attributeType);
            desc.attributes[i].offset = offset;
            desc.attributes[i].bufferIndex = 0;
            offset += getDataTypeLength(attribute.attributeType);
        }
        
        desc.layouts[0].stride = offset;
        desc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
        desc.layouts[0].stepRate = 1;
    }
    else
    {
        for (int i = 0; i < vertexFunc.vertexAttributes.count; i++)
        {
            auto attribute = vertexFunc.vertexAttributes[i];
            desc.attributes[i].format = getVertexFormat(attribute.attributeType);
            desc.attributes[i].offset = 0;
            desc.attributes[i].bufferIndex = i;
            desc.layouts[i].stride = getDataTypeLength(attribute.attributeType);
            desc.layouts[i].stepFunction = MTLVertexStepFunctionPerVertex;
            desc.layouts[i].stepRate = 1;
        }
    }
    
    return desc;
}

Program createProgramWithDesc(id<MTLDevice> device, const ProgramDesc& desc)
{
    NSError* error = nil;
    NSString* libraryPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:desc.library.c_str()]
                                                            ofType:@".metallib"];
    id<MTLLibrary> shaderLibrary = nil;
    
    if (libraryPath)
    {
        shaderLibrary = [device newLibraryWithFile:libraryPath
                                             error:&error];
    }
    else
    {
        std::vector<uint8_t> bytes;
        
        if (loadBinaryFile(desc.library, &bytes))
        {
            NSString* source = [NSString stringWithUTF8String:(const char*)bytes.data()];
            MTLCompileOptions* options = [[MTLCompileOptions alloc] init];
            shaderLibrary = [device newLibraryWithSource:source
                                                 options:options
                                                   error:&error];
        }
        else
        {
            NSLog(@"Failed to load shader source: %s", desc.library.c_str());
            assert(false);
            return {};
        }
    }
    
    id<MTLFunction> vertexFunc = [shaderLibrary newFunctionWithName:[NSString stringWithUTF8String:desc.vertexFunc.c_str()]];
    if (vertexFunc == nil)
    {
        NSLog(@"Failed to create vertex function: %s", desc.vertexFunc.c_str());
        assert(false);
        return {};
    }
    
    MTLFunctionConstantValues* constantValues = [[MTLFunctionConstantValues alloc] init];
    for (const auto& [name, val] : desc.constantsFloat)
    {
        [constantValues setConstantValue:&val
                                    type:MTLDataTypeFloat
                                withName:[NSString stringWithUTF8String:name.c_str()]];
    }
    for (const auto& [name, val] : desc.constantsVec4)
    {
        [constantValues setConstantValue:&val
                                    type:MTLDataTypeFloat4
                                withName:[NSString stringWithUTF8String:name.c_str()]];
    }
    
    
    id<MTLFunction> fragmentFunc = [shaderLibrary newFunctionWithName:[NSString stringWithUTF8String:desc.fragmentFunc.c_str()]
                                                       constantValues:constantValues
                                                                error:&error];
    if (fragmentFunc == nil)
    {
        NSLog(@"Failed to create vertex function: %s", desc.vertexFunc.c_str());
        NSLog(@"%@", [error description]);
        assert(false);
        return {};
    }
    
    MTLVertexDescriptor* vertexDesc = getVertexDesc(vertexFunc, false);
    assert(vertexDesc != nil);
    
    MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDesc.label = [NSString stringWithUTF8String:desc.name.c_str()];
    pipelineDesc.vertexFunction = vertexFunc;
    pipelineDesc.fragmentFunction = fragmentFunc;
    pipelineDesc.vertexDescriptor = vertexDesc;
    pipelineDesc.rasterSampleCount = 1;
    pipelineDesc.alphaToCoverageEnabled = NO;
    pipelineDesc.alphaToOneEnabled = NO;
    pipelineDesc.rasterizationEnabled = YES;
    pipelineDesc.depthAttachmentPixelFormat = (MTLPixelFormat)desc.depthPixelFormat;
    pipelineDesc.stencilAttachmentPixelFormat = (MTLPixelFormat)desc.stencilPixelFormat;
    
    for (int i = 0; i < desc.colorPixelFormats.size(); i++)
    {
        pipelineDesc.colorAttachments[i] = getBlendingDesc(desc.blendingEnables[i]);
        pipelineDesc.colorAttachments[i].pixelFormat = (MTLPixelFormat)desc.colorPixelFormats[i];
    }
    
    MTLRenderPipelineReflection* reflection = nil;
    id<MTLRenderPipelineState> pipeline = [device newRenderPipelineStateWithDescriptor:pipelineDesc
                                                                               options:MTLPipelineOptionArgumentInfo | MTLPipelineOptionBufferTypeInfo
                                                                            reflection:&reflection
                                                                                 error:&error];
    if (pipeline == nil || error)
    {
        NSLog(@"Failed to create render pipeline");
        NSLog(@"%@", [error description]);
        assert(false);
        return {};
    }
    
    MTLDepthStencilDescriptor* DSDesc = [[MTLDepthStencilDescriptor alloc] init];
    DSDesc.label = [NSString stringWithUTF8String:desc.name.c_str()];
    DSDesc.depthCompareFunction = desc.depthTestEnabled ? MTLCompareFunctionLessEqual : MTLCompareFunctionAlways;
    DSDesc.depthWriteEnabled = desc.depthWriteEnabled ? YES : NO;
    DSDesc.frontFaceStencil = getStencilDesc(desc.stencilTestEnabled, desc.stencilWriteEnabled);
    DSDesc.backFaceStencil = DSDesc.frontFaceStencil;
    
    id<MTLDepthStencilState> depthStencil = [device newDepthStencilStateWithDescriptor:DSDesc];
    assert(depthStencil != nil);
    
    Program program;
    program.vertexFunc = vertexFunc;
    program.fragmentFunc = fragmentFunc;
    program.reflection = reflection;
    program.renderPipelineState = pipeline;
    program.depthStencilState = depthStencil;
    
    return program;
}

Material createMaterialWithDesc(id<MTLCommandQueue> commandQueue, const MaterialDesc& desc)
{
    Material material;
    material.name = [NSString stringWithUTF8String:desc.name.c_str()];
    material.enabled = true;
    material.uniforms.values[MAT_FLAGS_NAME] = desc.uniforms.flags;
    material.uniforms.values[MAT_BASE_COLOR_NAME] = desc.uniforms.baseColor;
    material.uniforms.values[MAT_METALLIC_NAME] = desc.uniforms.metallic;
    material.uniforms.values[MAT_ROUGHNESS_NAME] = desc.uniforms.roughness;
    material.uniforms.values[MAT_EMISSIVE_NAME] = desc.uniforms.emissive;
    material.uniforms.values[MAT_AO_NAME] = desc.uniforms.ao;
    
    for (const auto& [index, path] : desc.textures)
    {
        std::string name = std::filesystem::path(path).filename();
        material.textures[index] = createTextureWithPath(commandQueue, path.c_str(), name.c_str());
    }
    
    return material;
}

int getAttributeIndex(const std::string& name, id<MTLFunction> vertexFunc)
{
    for (MTLVertexAttribute* attribute in vertexFunc.vertexAttributes)
    {
        if ([attribute.name isEqualToString:[NSString stringWithUTF8String:name.c_str()]])
            return (int)attribute.attributeIndex;
    }
    
    return -1;
}

MTLPrimitiveType getPrimitiveType(PrimitiveType type)
{
    switch (type)
    {
        case PRIM_Points: return MTLPrimitiveTypePoint;
        case PRIM_Lines: return MTLPrimitiveTypeLine;
        case PRIM_Triangles: return MTLPrimitiveTypeTriangle;
        default: assert(false); return MTLPrimitiveTypeTriangle;
    }
}
