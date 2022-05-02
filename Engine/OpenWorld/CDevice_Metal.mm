#include "CDevice_Metal.h"
#include "CDevice_Metal_Tools.h"

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
        model->meshes[i].material = createMaterialWithDesc(self->device, desc.meshes[i].material);
        
        int transformMatrixIndex = getResourceIndex(model->program.reflection, MODEL_MATRIX, "VS");
        model->meshes[i].ubos_VS[transformMatrixIndex] = [self->device newBufferWithBytes:&desc.meshes[i].transform
                                                                                   length:sizeof(glm::mat4)
                                                                                  options:MTLResourceStorageModeShared];
        model->meshes[i].ubos_VS[transformMatrixIndex].label = [NSString stringWithUTF8String:MODEL_MATRIX];
        
        for (const auto& [name, bufferDesc] : desc.meshes[i].attributes)
        {
            int index = getAttributeIndex(name, model->program.vertexFunc);
            model->meshes[i].attributes[index] = createBufferWithBytes(self->commandQueue,
                                                                       bufferDesc.data->data(),
                                                                       bufferDesc.data->size(),
                                                                       false,
                                                                       stringFormat("VAO.%s", name.c_str()).c_str());
            if (!desc.meshes[i].boundingBox.isValid() && name == VAO_POSITION)
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

void CDevice_Metal::drawModel(const IDeviceObject* _model, const Camera* camera)
{
    assert(self->renderEncoder);
    Model* model = (Model*)_model->object;
   
    [self->renderEncoder pushDebugGroup:model->name];
    [self->renderEncoder setRenderPipelineState:model->program.renderPipelineState];
    [self->renderEncoder setDepthStencilState:model->program.depthStencilState];
    
    if (camera)
    {
        [self->renderEncoder pushDebugGroup:@"Setup Camera"];
        [self->renderEncoder setVertexBytes:&camera->viewMatrix()
                                     length:sizeof(glm::mat4)
                                    atIndex:getResourceIndex(model->program.reflection, VIEW_MATRIX, "VS")];
        [self->renderEncoder setVertexBytes:&camera->projectionMatrix()
                                     length:sizeof(glm::mat4)
                                    atIndex:getResourceIndex(model->program.reflection, PROJECTION_MATRIX, "VS")];
        [self->renderEncoder popDebugGroup];
    }
    
    for (const auto& mesh : model->meshes)
    {
        [self->renderEncoder pushDebugGroup:mesh.name];
        
        if (!mesh.ubos_VS.empty())
        {
            [self->renderEncoder pushDebugGroup:@"Setup UBO"];
            for (const auto& [index, buffer] : mesh.ubos_VS)
            {
                [self->renderEncoder setVertexBuffer:buffer
                                              offset:0
                                             atIndex:index];
            }
            [self->renderEncoder popDebugGroup];
        }
        
        if (mesh.material.enabled)
        {
            [self->renderEncoder pushDebugGroup:@"Setup Material"];
            
            [self->renderEncoder popDebugGroup];
        }
        
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
