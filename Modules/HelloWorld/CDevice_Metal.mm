#include "CDevice_Metal.h"

struct VAO
{
    id<MTLBuffer> positions;
    id<MTLBuffer> normals;
    id<MTLBuffer> tangents;
    id<MTLBuffer> texcoords;
};

struct FBO
{
    std::vector<id<MTLTexture>> colorAttachments;
    id<MTLTexture> depthAttachment;
    id<MTLTexture> stencilAttachment;
    MTLRenderPassDescriptor* renderPassDesc = nullptr;
};

struct SPO
{
    id<MTLFunction> vertexFunc;
    id<MTLFunction> fragmentFunc;
    MTLRenderPipelineReflection* reflection = nullptr;
    id<MTLRenderPipelineState> renderPipelineState;
    id<MTLDepthStencilState> depthStencilState;
    std::map<std::string, std::map<int, id<MTLBuffer>>> buffers;
    std::map<std::string, std::map<int, id<MTLTexture>>> textures;
};

struct CDevice_Metal::PrivateData
{
    MTKView* view = nullptr;
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<MTLCommandBuffer> commandBuffer;
    id<MTLRenderCommandEncoder> renderEncoder;
    id<MTLBlitCommandEncoder> copyEncoder;
    id<MTLComputeCommandEncoder> computerEncoder;
    std::unordered_map<void*, size_t> types;
    MTLRenderPassDescriptor* renderPassDesc = nullptr;
};

IDeviceObject::~IDeviceObject()
{
    if (object && device)
        device->release(object);
    object = nullptr;
    device = nullptr;
}

CDevice_Metal::CDevice_Metal(void* view)
    : _data(std::make_shared<CDevice_Metal::PrivateData>())
{
    _data->view = (__bridge MTKView*)view;
    _data->view.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    _data->view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    _data->view.clearColor = MTLClearColorMake(0.129, 0.156, 0.188, 1.0);
    _data->view.clearDepth = 1.0;
    _data->view.clearStencil = 0;
    _data->device = _data->view.device;
    _data->commandQueue = [_data->device newCommandQueue];
}

IDeviceObject::ARC CDevice_Metal::createBufferWithBytes(const void* data,
                                                        int size,
                                                        bool sharedStorageMode,
                                                        const char* label)
{
    id<MTLBuffer> buffer = nil;
    
    if (data)
    {
        if (sharedStorageMode)
        {
            buffer = [_data->device newBufferWithBytes:data
                                                length:size
                                               options:MTLResourceStorageModeShared];
        }
        else
        {
            id<MTLCommandBuffer> commandBuffer = [_data->commandQueue commandBuffer];
            id<MTLBlitCommandEncoder> copyEncoder = [commandBuffer blitCommandEncoder];
            id<MTLBuffer> staging = [_data->device newBufferWithBytes:data
                                                               length:size
                                                              options:MTLResourceStorageModeShared];
            buffer = [_data->device newBufferWithLength:size
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
        
        buffer = [_data->device newBufferWithLength:size
                                            options:options];
    }
    
    if (label)
    {
        [buffer setLabel:[NSString stringWithUTF8String:label]];
    }
    
    _data->types[(__bridge void*)buffer] = typeid(id<MTLBuffer>).hash_code();
    
    return IDeviceObject::create((__bridge_retained void*)buffer, this);
}

IDeviceObject::ARC CDevice_Metal::createTextureWithDesc(const TextureDesc& desc, const char* label)
{
    MTLTextureUsage usage = 0;
    if (strchr(desc.usage, 'R')) usage |= MTLTextureUsageShaderRead;
    if (strchr(desc.usage, 'W')) usage |= MTLTextureUsageShaderWrite;
    if (strchr(desc.usage, 'T')) usage |= MTLTextureUsageRenderTarget;
    
    MTLTextureDescriptor* descriptor = [[MTLTextureDescriptor alloc] init];
    descriptor.textureType = MTLTextureType2D;
    descriptor.pixelFormat = (MTLPixelFormat)desc.pixelFormat;
    descriptor.width = desc.width;
    descriptor.height = desc.height;
    descriptor.depth = 1;
    descriptor.mipmapLevelCount = 1;
    descriptor.sampleCount = 1;
    descriptor.arrayLength = 1;
    descriptor.storageMode = desc.sharedStorageMode ? MTLStorageModeShared : MTLStorageModePrivate;
    descriptor.usage = usage;

    id<MTLTexture> texture = [_data->device newTextureWithDescriptor:descriptor];
    if (label) [texture setLabel:[NSString stringWithUTF8String:label]];
    
    IDeviceObject::ARC object = IDeviceObject::create((__bridge_retained void*)texture, this);
    replaceTextureRegion(object, Int2(0), Int2(0), desc.src);
    
    return object;
}

IDeviceObject::ARC CDevice_Metal::createVAOWithDesc(const VAODesc& desc)
{
    VAO* vao = new VAO();
    vao->positions = (__bridge id)desc.positions->object;
    vao->normals = (__bridge id)desc.normals->object;
    vao->tangents = (__bridge id)desc.tangents->object;
    vao->texcoords = (__bridge id)desc.texcoords->object;
    
    _data->types[vao] = typeid(VAO).hash_code();
    
    return IDeviceObject::create(vao, this);
}

IDeviceObject::ARC CDevice_Metal::createFBOWithDesc(const FBODesc& desc)
{
    FBO* fbo = new FBO();
    
    // Store attachments
    fbo->colorAttachments.resize(desc.colorAttachments.size());
    for (int i = 0; i < desc.colorAttachments.size(); i++)
    {
        fbo->colorAttachments[i] = (__bridge id)desc.colorAttachments[i].texture->object;
    }
    fbo->depthAttachment = (__bridge id)desc.depthAttachment.texture->object;
    fbo->stencilAttachment = (__bridge id)desc.stencilAttachment.texture->object;
    
    // Create render pass descriptor
    fbo->renderPassDesc = [[MTLRenderPassDescriptor alloc] init];
    for (int i = 0; i < desc.colorAttachments.size(); i++)
    {
        fbo->renderPassDesc.colorAttachments[i].texture = fbo->colorAttachments[i];
        fbo->renderPassDesc.colorAttachments[i].loadAction = desc.colorAttachments[i].loadActionClear ? MTLLoadActionClear : MTLLoadActionLoad;
        fbo->renderPassDesc.colorAttachments[i].storeAction = desc.colorAttachments[i].storeActionDontCare ? MTLStoreActionDontCare : MTLStoreActionStore;
        
        if (desc.colorAttachments[i].loadActionClear)
        {
            glm::vec4 clearColor = std::get<glm::vec4>(desc.colorAttachments[i].clearValue);
            fbo->renderPassDesc.colorAttachments[i].clearColor = MTLClearColorMake(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        }
    }
    
    if (fbo->depthAttachment)
    {
        fbo->renderPassDesc.depthAttachment.texture = fbo->depthAttachment;
        fbo->renderPassDesc.depthAttachment.loadAction = desc.depthAttachment.loadActionClear ? MTLLoadActionClear : MTLLoadActionLoad;
        fbo->renderPassDesc.depthAttachment.storeAction = desc.depthAttachment.storeActionDontCare ? MTLStoreActionDontCare : MTLStoreActionStore;
        
        if (desc.depthAttachment.loadActionClear)
        {
            float clearDepth = std::get<float>(desc.depthAttachment.clearValue);
            fbo->renderPassDesc.depthAttachment.clearDepth = clearDepth;
        }
    }
    
    if (fbo->stencilAttachment)
    {
        fbo->renderPassDesc.stencilAttachment.texture = fbo->stencilAttachment;
        fbo->renderPassDesc.stencilAttachment.loadAction = desc.stencilAttachment.loadActionClear ? MTLLoadActionClear : MTLLoadActionLoad;
        fbo->renderPassDesc.stencilAttachment.storeAction = desc.stencilAttachment.storeActionDontCare ? MTLStoreActionDontCare : MTLStoreActionStore;
        
        if (desc.stencilAttachment.loadActionClear)
        {
            float clearStencil = std::get<int>(desc.depthAttachment.clearValue);
            fbo->renderPassDesc.stencilAttachment.clearStencil = clearStencil;
        }
    }
    
    _data->types[fbo] = typeid(FBO).hash_code();
    
    return IDeviceObject::create(fbo, this);
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

int findResourceIndex(MTLRenderPipelineReflection* reflection,
                      std::string name,
                      std::string stage)
{
    NSArray<MTLArgument*>* argumentList = (stage == "VS" ? reflection.vertexArguments :
                                                           reflection.fragmentArguments);
    for (MTLArgument* argument in argumentList)
    {
        if ([argument.name isEqualToString:[NSString stringWithUTF8String:name.c_str()]])
        {
            return (int)argument.index;
        }
    }
    
    return -1;
}

std::map<std::string, std::map<int, id<MTLBuffer>>> resolveBufferBindings(MTLRenderPipelineReflection* reflection,
                                                                          const SPODesc& desc)
{
    std::map<std::string, std::map<int, id<MTLBuffer>>> buffers;
    
    for (const auto& [name, object] : desc.buffers)
    {
        id<MTLBuffer> buffer = (__bridge id<MTLBuffer>)object->object;
        int index1 = findResourceIndex(reflection, name, "VS");
        int index2 = findResourceIndex(reflection, name, "FS");
        
        if (index1 >= 0) {
            buffers["VS"][index1] = buffer;
        }
        
        if (index2 >= 0) {
            buffers["FS"][index2] = buffer;
        }
    }
    
    return buffers;
}

std::map<std::string, std::map<int, id<MTLTexture>>> resolveTextureBindings(MTLRenderPipelineReflection* reflection,
                                                                            const SPODesc& desc)
{
    std::map<std::string, std::map<int, id<MTLTexture>>> textures;
    
    for (const auto& [name, object] : desc.textures)
    {
        id<MTLTexture> texture = (__bridge id<MTLTexture>)object->object;
        int index1 = findResourceIndex(reflection, name, "VS");
        int index2 = findResourceIndex(reflection, name, "FS");
        
        if (index1 >= 0) {
            textures["VS"][index1] = texture;
        }
        
        if (index2 >= 0) {
            textures["FS"][index2] = texture;
        }
    }
    
    return textures;
}

void checkResourceBindings(SPO* spo)
{
    std::unordered_map<std::string, std::string> missings;
    
    for (MTLArgument* argument in spo->reflection.vertexArguments)
    {
        if (argument.type == MTLArgumentTypeBuffer)
        {
            auto it = spo->buffers.find("VS");
            if (it == spo->buffers.end() ||
                it->second.find((int)argument.index) == it->second.end())
            {
                missings["VS"] = [argument.name cStringUsingEncoding:NSUTF8StringEncoding];
            }
        }
        
        if (argument.type == MTLArgumentTypeTexture)
        {
            auto it = spo->textures.find("VS");
            if (it == spo->textures.end() ||
                it->second.find((int)argument.index) == it->second.end())
            {
                missings["VS"] = [argument.name cStringUsingEncoding:NSUTF8StringEncoding];
            }
        }
    }
    
    for (MTLArgument* argument in spo->reflection.fragmentArguments)
    {
        if (argument.type == MTLArgumentTypeBuffer)
        {
            auto it = spo->buffers.find("FS");
            if (it == spo->buffers.end() ||
                it->second.find((int)argument.index) == it->second.end())
            {
                missings["FS"] = [argument.name cStringUsingEncoding:NSUTF8StringEncoding];
            }
        }
        
        if (argument.type == MTLArgumentTypeTexture)
        {
            auto it = spo->textures.find("FS");
            if (it == spo->textures.end() ||
                it->second.find((int)argument.index) == it->second.end())
            {
                missings["FS"] = [argument.name cStringUsingEncoding:NSUTF8StringEncoding];
            }
        }
    }
    
    for (const auto& [stage, name] : missings)
    {
        printf("[%s] %s requires resource binding.\n", stage.c_str(), name.c_str());
    }
}

IDeviceObject::ARC CDevice_Metal::createSPOWithDesc(const SPODesc& desc)
{
    NSError* error = nil;
    NSString* libraryPath = [[NSBundle mainBundle] pathForResource:@"default" ofType:@".metallib"];
    id<MTLLibrary> shaderLibrary = [_data->device newLibraryWithFile:libraryPath
                                                               error:&error];
    id<MTLFunction> vertexFunc = [shaderLibrary newFunctionWithName:[NSString stringWithUTF8String:desc.vertexFunc.c_str()]];
    id<MTLFunction> fragmentFunc = [shaderLibrary newFunctionWithName:[NSString stringWithUTF8String:desc.fragmentFunc.c_str()]];
    
    MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDesc.label = [NSString stringWithUTF8String:desc.label.c_str()];
    pipelineDesc.vertexFunction = vertexFunc;
    pipelineDesc.fragmentFunction = fragmentFunc;
    pipelineDesc.vertexDescriptor = 0;
    pipelineDesc.rasterSampleCount = 1;
    pipelineDesc.alphaToCoverageEnabled = NO;
    pipelineDesc.alphaToOneEnabled = NO;
    pipelineDesc.rasterizationEnabled = YES;
    pipelineDesc.depthAttachmentPixelFormat = (MTLPixelFormat)desc.depthPixelFormat;
    pipelineDesc.stencilAttachmentPixelFormat = (MTLPixelFormat)desc.stencilPixelFormat;
    
    for (int i = 0; i < desc.colorPixelFormats.size(); i++)
    {
        pipelineDesc.colorAttachments[i] = getBlendingDesc(desc.blendingEnabled);
        pipelineDesc.colorAttachments[i].pixelFormat = (MTLPixelFormat)desc.colorPixelFormats[i];
    }
    
    MTLRenderPipelineReflection* reflection = nil;
    id<MTLRenderPipelineState> pipeline = [_data->device newRenderPipelineStateWithDescriptor:pipelineDesc
                                                                                      options:MTLPipelineOptionArgumentInfo | MTLPipelineOptionBufferTypeInfo
                                                                                   reflection:&reflection
                                                                                        error:&error];
    assert(pipeline != nil);
    
    MTLDepthStencilDescriptor* DSDesc = [[MTLDepthStencilDescriptor alloc] init];
    DSDesc.label = [NSString stringWithUTF8String:desc.label.c_str()];
    DSDesc.depthCompareFunction = desc.depthTestEnabled ? MTLCompareFunctionLess : MTLCompareFunctionAlways;
    DSDesc.depthWriteEnabled = desc.depthWriteEnabled ? YES : NO;
    DSDesc.frontFaceStencil = getStencilDesc(desc.stencilTestEnabled, desc.stencilWriteEnabled);
    DSDesc.backFaceStencil = DSDesc.frontFaceStencil;
    
    id<MTLDepthStencilState> depthStencil = [_data->device newDepthStencilStateWithDescriptor:DSDesc];
    assert(depthStencil != nil);
    
    SPO* spo = new SPO();
    spo->vertexFunc = vertexFunc;
    spo->fragmentFunc = fragmentFunc;
    spo->reflection = reflection;
    spo->renderPipelineState = pipeline;
    spo->depthStencilState = depthStencil;
    spo->buffers = resolveBufferBindings(reflection, desc);
    spo->textures = resolveTextureBindings(reflection, desc);
    
    checkResourceBindings(spo);
    _data->types[spo] = typeid(SPO).hash_code();
    
    return IDeviceObject::create(spo, this);
}

void CDevice_Metal::release(void* object)
{
    size_t hash = _data->types.at(object);
    _data->types.erase(object);
    
    if (hash == typeid(id<MTLBuffer>).hash_code())
    {
        id<MTLBuffer> buffer = (__bridge_transfer id<MTLBuffer>)object;
        buffer = nil;
    }
    else if (hash == typeid(id<MTLTexture>).hash_code())
    {
        id<MTLTexture> texture = (__bridge_transfer id<MTLTexture>)object;
        texture = nil;
    }
    else if (hash == typeid(VAO).hash_code())
    {
        delete (VAO*)object;
    }
    else if (hash == typeid(FBO).hash_code())
    {
        delete (FBO*)object;
    }
    else if (hash == typeid(SPO).hash_code())
    {
        delete (SPO*)object;
    }
    else
    {
        assert(false);
    }
}

void CDevice_Metal::replaceBufferRange(IDeviceObject::ARC _buffer,
                                       int offset,
                                       int size,
                                       const void* src)
{}

void CDevice_Metal::replaceTextureRegion(IDeviceObject::ARC _texture,
                                         Int2 offset,
                                         Int2 size,
                                         const void* src)
{
    id<MTLTexture> texture = (__bridge id)_texture->object;
    if (size.x == 0 || size.y == 0)
        size = Int2(texture.width, texture.height);
    
    id<MTLCommandBuffer> commandBuffer = nil;
    id<MTLBlitCommandEncoder> copyEncoder = nil;
    id<MTLBuffer> staging = [_data->device newBufferWithBytes:src
                                                       length:size.x * size.y * sizeof(int)
                                                      options:MTLResourceStorageModeShared];
    if (_data->copyEncoder)
    {
        copyEncoder = _data->copyEncoder;
    }
    else
    {
        commandBuffer = [_data->commandQueue commandBuffer];
        copyEncoder = [commandBuffer blitCommandEncoder];
    }
    
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

void CDevice_Metal::readbackBufferRange(IDeviceObject::ARC buffer,
                                        int offset,
                                        int size,
                                        void* dst)
{}

void CDevice_Metal::readbackTextureRegion(IDeviceObject::ARC texture,
                                          Int2 offset,
                                          Int2 size,
                                          void* dst)
{}


void CDevice_Metal::beginEncoding(CommandType type)
{
    if (_data->commandBuffer == nil)
        _data->commandBuffer = [_data->commandQueue commandBuffer];
    
    switch (type)
    {
        case CMD_Render:
        {
            MTLRenderPassDescriptor* renderPassDesc = _data->renderPassDesc;
            if (renderPassDesc == nullptr) {
                renderPassDesc = _data->view.currentRenderPassDescriptor;
            }
            _data->renderEncoder = [_data->commandBuffer renderCommandEncoderWithDescriptor:renderPassDesc];
            break;
        }
        case CMD_Copy:
        {
            _data->copyEncoder = [_data->commandBuffer blitCommandEncoder];
            break;
        }
        case CMD_Compute:
        {
            _data->computerEncoder = [_data->commandBuffer computeCommandEncoder];
            break;
        }
    }
}

void CDevice_Metal::endEncoding()
{
    if (_data->renderEncoder)
    {
        [_data->renderEncoder endEncoding];
    }
    
    if (_data->copyEncoder)
    {
        [_data->copyEncoder endEncoding];
    }
    
    if (_data->computerEncoder)
    {
        [_data->computerEncoder endEncoding];
    }
}

void CDevice_Metal::beginDebugLabel(const char* label)
{
    if (_data->renderEncoder)
    {
        [_data->renderEncoder pushDebugGroup:[NSString stringWithUTF8String:label]];
    }
    
    if (_data->copyEncoder)
    {
        [_data->copyEncoder pushDebugGroup:[NSString stringWithUTF8String:label]];
    }
    
    if (_data->computerEncoder)
    {
        [_data->computerEncoder pushDebugGroup:[NSString stringWithUTF8String:label]];
    }
}

void CDevice_Metal::endDebugLabel()
{
    if (_data->renderEncoder)
    {
        [_data->renderEncoder popDebugGroup];
    }
    
    if (_data->copyEncoder)
    {
        [_data->copyEncoder popDebugGroup];
    }
    
    if (_data->computerEncoder)
    {
        [_data->computerEncoder popDebugGroup];
    }
}

void CDevice_Metal::draw(IDeviceObject::ARC indexBuffer,
                         int indexCount,
                         int indexOffset,
                         int indexTypeBits,
                         int primitiveType)
{
    assert(_data->renderEncoder);
    
    [_data->renderEncoder drawIndexedPrimitives:(MTLPrimitiveType)primitiveType
                                     indexCount:indexCount
                                      indexType:indexTypeBits==32 ? MTLIndexTypeUInt32 : MTLIndexTypeUInt16
                                    indexBuffer:(__bridge id<MTLBuffer>)indexBuffer->object
                              indexBufferOffset:indexOffset];
}

void CDevice_Metal::present(void* _view)
{
    MTKView* view = (__bridge MTKView*)_view;
    id<MTLCommandBuffer> commandBuffer = _data->commandBuffer;
    
    if (commandBuffer == nil)
        commandBuffer = [_data->commandQueue commandBuffer];
    
    [commandBuffer presentDrawable:view.currentDrawable];
    
    if (_data->commandBuffer == nil)
        [commandBuffer commit];
}

void CDevice_Metal::commit()
{
    if (_data->commandBuffer)
    {
        [_data->commandBuffer commit];
        _data->commandBuffer = nil;
    }
}

void CDevice_Metal::setVAO(IDeviceObject::ARC _vao)
{
    VAO* vao = (VAO*)_vao->object;
    [_data->renderEncoder setVertexBuffer:vao->positions
                                   offset:0
                                  atIndex:VBO_Position];
    
    if (vao->normals)
    {
        [_data->renderEncoder setVertexBuffer:vao->normals
                                       offset:0
                                      atIndex:VBO_Normal];
    }
    
    if (vao->tangents)
    {
        [_data->renderEncoder setVertexBuffer:vao->tangents
                                       offset:0
                                      atIndex:VBO_Tangent];
    }
    
    if (vao->texcoords)
    {
        [_data->renderEncoder setVertexBuffer:vao->texcoords
                                       offset:0
                                      atIndex:VBO_Texcoord];
    }
}

void CDevice_Metal::setFBO(IDeviceObject::ARC _fbo)
{
    _data->renderPassDesc = ((FBO*)_fbo->object)->renderPassDesc;
}

void CDevice_Metal::setSPO(IDeviceObject::ARC _spo)
{
    SPO* spo = (SPO*)_spo->object;
    
    [_data->renderEncoder setRenderPipelineState:spo->renderPipelineState];
    [_data->renderEncoder setDepthStencilState:spo->depthStencilState];
    
    for (auto const& [stage, bindings] : spo->buffers)
    {
        if (stage == "VS")
        {
            for (const auto& [index, buffer] : bindings)
            {
                [_data->renderEncoder setVertexBuffer:buffer
                                               offset:0
                                              atIndex:index];
            }
        }
        
        if (stage == "FS")
        {
            for (const auto& [index, buffer] : bindings)
            {
                [_data->renderEncoder setFragmentBuffer:buffer
                                                 offset:0
                                                atIndex:index];
            }
        }
    }
    
    for (auto const& [stage, bindings] : spo->textures)
    {
        if (stage == "VS")
        {
            for (const auto& [index, texture] : bindings)
            {
                [_data->renderEncoder setVertexTexture:texture
                                               atIndex:index];
            }
        }
        
        if (stage == "FS")
        {
            for (const auto& [index, texture] : bindings)
            {
                [_data->renderEncoder setFragmentTexture:texture
                                                 atIndex:index];
            }
        }
    }
}

Int2 drawableSizeWithMTKView(void* _view)
{
    Int2 result;
    
    MTKView* view = (__bridge MTKView*)_view;
    result.x = view.drawableSize.width;
    result.y = view.drawableSize.height;
    
    return result;
}
