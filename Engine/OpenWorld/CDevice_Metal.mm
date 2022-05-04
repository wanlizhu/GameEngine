#include "CDevice_Metal.h"
#include "MetalTools.h"

struct CDevice_Metal::Context
{
    MTKView* view = nullptr;
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<MTLCommandBuffer> commandBuffer;
    id<MTLRenderCommandEncoder> renderEncoder;
    MTLRenderPassDescriptor* renderPassDesc = nullptr;
    std::unordered_map<void*, size_t> types;
};

IDeviceObject::~IDeviceObject()
{
    if (object && device)
        device->release(object);
    object = nullptr;
    device = nullptr;
}

CDevice_Metal::CDevice_Metal(void* view)
    : self(std::make_shared<CDevice_Metal::Context>())
{
    self->view = (__bridge MTKView*)view;
    self->view.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    self->view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    self->view.clearColor = MTLClearColorMake(0.129, 0.156, 0.188, 1.0);
    self->view.clearDepth = 1.0;
    self->view.clearStencil = 0;
    self->device = self->view.device;
    self->commandQueue = [self->device newCommandQueue];
}

BoundingBox getBoundingBox(const glm::vec3* positions, int count)
{
    BoundingBox box;
    
    for (int i = 0; i < count; i++)
    {
        box.expand(positions[i]);
    }
    
    return box;
}

IDeviceObject::ARC CDevice_Metal::createModelWithDesc(const ModelDesc& desc)
{
    Model* model = new Model();
    
    model->name = [NSString stringWithUTF8String:desc.name.c_str()];
    model->program = createProgramWithDesc(self->device, desc.program);
    model->meshes.resize(desc.meshes.size());
    
    for (int i = 0; i < (int)desc.meshes.size(); i++)
    {
        model->meshes[i].name = [NSString stringWithUTF8String:desc.meshes[i].name.c_str()];
        model->meshes[i].material = createMaterialWithDesc(self->commandQueue, desc.meshes[i].material);
        model->meshes[i].transform = desc.meshes[i].transform;
        
        for (const auto& [index, bufferDesc] : desc.meshes[i].attributes)
        {
            model->meshes[i].attributes[index] = createBufferWithBytes(self->commandQueue,
                                                                       bufferDesc.data->data(),
                                                                       bufferDesc.data->size(),
                                                                       false,
                                                                       stringFormat("VBO[%d]", index).c_str());
            if (!desc.meshes[i].boundingBox.isValid() && index == VAO_POSITION_INDEX)
            {
                model->meshes[i].boundingBox = getBoundingBox((const glm::vec3*)bufferDesc.data->data(),
                                                              bufferDesc.data->size() / sizeof(glm::vec3));
            }
        }
        
        assert(model->meshes[i].boundingBox.isValid());
        model->boundingBox.expand(model->meshes[i].boundingBox);
        
        for (int j = 0; j < desc.meshes[i].indices.size(); j++)
        {
            const auto& indexBufferDesc = desc.meshes[i].indices[j];
            IndexDraw indexDraw;
            indexDraw.primitiveType = getPrimitiveType(indexBufferDesc.primitiveType);
            indexDraw.indexCount = indexBufferDesc.elementCount;
            indexDraw.indexOffset = 0;
            indexDraw.indexType = indexBufferDesc.indexTypeBites == 32 ? MTLIndexTypeUInt32 : MTLIndexTypeUInt16;
            indexDraw.indexBuffer = createBufferWithBytes(self->commandQueue,
                                                          indexBufferDesc.data->data(),
                                                          indexBufferDesc.data->size(),
                                                          false,
                                                          stringFormat("IBO[%d]", j).c_str());
            model->meshes[i].indices.push_back(indexDraw);
        }
    }
    
    self->types[model] = typeid(Model).hash_code();
    
    return IDeviceObject::create(model, this);
}

BoundingBox CDevice_Metal::boundingBox(void* object)
{
    size_t hash = self->types.at(object);
    self->types.erase(object);
    
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

void CDevice_Metal::release(void* object)
{
    size_t hash = self->types.at(object);
    self->types.erase(object);
    
    if (hash == typeid(Model).hash_code())
    {
        delete (Model*)object;
    }
    else
    {
        assert(false);
    }
}

void CDevice_Metal::beginEncoding()
{
    if (self->commandBuffer == nil)
        self->commandBuffer = [self->commandQueue commandBuffer];
    
    MTLRenderPassDescriptor* renderPassDesc = self->renderPassDesc;
    
    if (renderPassDesc == nullptr)
    {
        renderPassDesc = self->view.currentRenderPassDescriptor;
    }
    
    self->renderEncoder = [self->commandBuffer renderCommandEncoderWithDescriptor:renderPassDesc];
}

void CDevice_Metal::endEncoding()
{
    if (self->renderEncoder)
    {
        [self->renderEncoder endEncoding];
    }
}

void setCameraUniforms(id<MTLRenderCommandEncoder> renderEncoder,
                       MTLRenderPipelineReflection* reflection,
                       const Camera* camera)
{
    if (camera)
    {
        [renderEncoder pushDebugGroup:@"Setup Camera Uniforms"];
        [renderEncoder setVertexBytes:&camera->viewMatrix()
                               length:sizeof(glm::mat4)
                              atIndex:VIEW_MATRIX_INDEX];
        [renderEncoder setVertexBytes:&camera->projectionMatrix()
                               length:sizeof(glm::mat4)
                              atIndex:PROJECTION_MATRIX_INDEX];
        [renderEncoder setFragmentBytes:&camera->position()
                                 length:sizeof(glm::vec3)
                                atIndex:CAMERA_POSITION_INDEX];
        [renderEncoder popDebugGroup];
    }
}

void setLightUniforms(id<MTLRenderCommandEncoder> renderEncoder,
                      MTLRenderPipelineReflection* reflection,
                      const Light* light)
{
    if (light)
    {
        [renderEncoder pushDebugGroup:@"Setup Light Uniforms"];
        [renderEncoder setFragmentBytes:&light->position()
                                 length:sizeof(glm::vec3)
                                atIndex:LIGHT_POSITION_INDEX];
        [renderEncoder popDebugGroup];
    }
}

void setMeshUniforms(id<MTLRenderCommandEncoder> renderEncoder,
                     MTLRenderPipelineReflection* reflection,
                     const glm::mat4& modelMatrix)
{
    [renderEncoder pushDebugGroup:@"Setup Mesh Uniforms"];
    [renderEncoder setVertexBytes:&modelMatrix
                           length:sizeof(glm::mat4)
                          atIndex:MODEL_MATRIX_INDEX];
    [renderEncoder popDebugGroup];
}

int getFragmentBufferLength(MTLRenderPipelineReflection* reflection,
                            int index)
{
    for (MTLArgument* argument in reflection.fragmentArguments)
    {
        if (argument.index == index)
        {
            return argument.bufferDataSize;
        }
    }
    
    return 0;
}

int getFragmentBufferMemberOffset(MTLRenderPipelineReflection* reflection,
                                  int index,
                                  const char* memberName)
{
    MTLStructType* bufferStructType = nil;
    
    for (MTLArgument* argument in reflection.fragmentArguments)
    {
        if (argument.index == index)
        {
            bufferStructType = argument.bufferStructType;
            break;
        }
    }
    
    assert(bufferStructType);
    MTLStructMember* member = [bufferStructType memberByName:[NSString stringWithUTF8String:memberName]];
    
    return member.offset;
}

void setMaterialUniforms(id<MTLRenderCommandEncoder> renderEncoder,
                         id<MTLCommandQueue> commandQueue,
                         MTLRenderPipelineReflection* reflection,
                         Material& material)
{
    if (material.enabled)
    {
        if (material.uniforms.ubo == nil)
        {
            material.uniforms.ubo = createBufferWithBytes(commandQueue,
                                                          nullptr,
                                                          getFragmentBufferLength(reflection, MATERIAL_BLOCK_INDEX),
                                                          true,
                                                          [material.name cStringUsingEncoding:NSUTF8StringEncoding]);
            std::unordered_map<std::string, int> offsets;
            offsets[MAT_FLAGS_NAME] = getFragmentBufferMemberOffset(reflection, MATERIAL_BLOCK_INDEX, MAT_FLAGS_NAME);
            offsets[MAT_BASE_COLOR_NAME] = getFragmentBufferMemberOffset(reflection, MATERIAL_BLOCK_INDEX, MAT_BASE_COLOR_NAME);
            offsets[MAT_METALLIC_NAME] = getFragmentBufferMemberOffset(reflection, MATERIAL_BLOCK_INDEX, MAT_METALLIC_NAME);
            offsets[MAT_ROUGHNESS_NAME] = getFragmentBufferMemberOffset(reflection, MATERIAL_BLOCK_INDEX, MAT_ROUGHNESS_NAME);
            offsets[MAT_EMISSIVE_NAME] = getFragmentBufferMemberOffset(reflection, MATERIAL_BLOCK_INDEX, MAT_EMISSIVE_NAME);
            offsets[MAT_AO_NAME] = getFragmentBufferMemberOffset(reflection, MATERIAL_BLOCK_INDEX, MAT_AO_NAME);
            
            uint8_t* buffer = (uint8_t*)material.uniforms.ubo.contents;
            *(int*)(buffer + offsets[MAT_FLAGS_NAME]) = std::any_cast<int>(material.uniforms.values[MAT_FLAGS_NAME]);
            *(float3*)(buffer + offsets[MAT_BASE_COLOR_NAME]) = std::any_cast<float3>(material.uniforms.values[MAT_BASE_COLOR_NAME]);
            *(float*)(buffer + offsets[MAT_METALLIC_NAME]) = std::any_cast<float>(material.uniforms.values[MAT_METALLIC_NAME]);
            *(float*)(buffer + offsets[MAT_ROUGHNESS_NAME]) = std::any_cast<float>(material.uniforms.values[MAT_ROUGHNESS_NAME]);
            *(float3*)(buffer + offsets[MAT_EMISSIVE_NAME]) = std::any_cast<float3>(material.uniforms.values[MAT_EMISSIVE_NAME]);
            *(float*)(buffer + offsets[MAT_AO_NAME]) = std::any_cast<float>(material.uniforms.values[MAT_AO_NAME]);
        }
        
        
        [renderEncoder pushDebugGroup:@"Setup Material Uniforms"];
        [renderEncoder setFragmentBuffer:material.uniforms.ubo
                                  offset:0
                                 atIndex:MATERIAL_BLOCK_INDEX];
        [renderEncoder popDebugGroup];
    }
}

void CDevice_Metal::drawModel(const IDeviceObject* _model,
                              const Light* light,
                              const Camera* camera)
{
    assert(self->renderEncoder);
    Model* model = (Model*)_model->object;
   
    [self->renderEncoder pushDebugGroup:model->name];
    [self->renderEncoder setRenderPipelineState:model->program.renderPipelineState];
    [self->renderEncoder setDepthStencilState:model->program.depthStencilState];
    
    setCameraUniforms(self->renderEncoder,
                      model->program.reflection,
                      camera);
    setLightUniforms(self->renderEncoder,
                     model->program.reflection,
                     light);
    
    for (auto& mesh : model->meshes)
    {
        [self->renderEncoder pushDebugGroup:mesh.name];
        
        setMaterialUniforms(self->renderEncoder,
                            self->commandQueue,
                            model->program.reflection,
                            mesh.material);
        setMeshUniforms(self->renderEncoder,
                        model->program.reflection,
                        mesh.transform);
        
        if (!mesh.attributes.empty())
        {
            [self->renderEncoder pushDebugGroup:@"Setup VAO"];
            for (const auto& [index, buffer] : mesh.attributes)
            {
                [self->renderEncoder setVertexBuffer:buffer
                                              offset:0
                                             atIndex:index];
            }
            [self->renderEncoder popDebugGroup];
        }
        
        for (const auto& indexDraw : mesh.indices)
        {
            [self->renderEncoder drawIndexedPrimitives:indexDraw.primitiveType
                                            indexCount:indexDraw.indexCount
                                             indexType:indexDraw.indexType
                                           indexBuffer:indexDraw.indexBuffer
                                     indexBufferOffset:indexDraw.indexOffset
                                         instanceCount:1];
        }
        
        [self->renderEncoder popDebugGroup];
    }
    
    [self->renderEncoder popDebugGroup];
}

void CDevice_Metal::present(void* _view)
{
    MTKView* view = (__bridge MTKView*)_view;
    id<MTLCommandBuffer> commandBuffer = self->commandBuffer;
    
    if (commandBuffer == nil)
        commandBuffer = [self->commandQueue commandBuffer];
    
    [commandBuffer presentDrawable:view.currentDrawable];
    
    if (self->commandBuffer == nil)
        [commandBuffer commit];
}

void CDevice_Metal::commit()
{
    if (self->commandBuffer)
    {
        [self->commandBuffer commit];
        self->commandBuffer = nil;
    }
}
