#include "VkModelZHU.h"
#include "VkDeviceZHU.h"

VkModelZHU::VkModelZHU(VkDeviceZHU* device, const VkModelDB& model)
    : _device(device)
{
    _pipeline = make_vulkan_pipeline();
    _descriptor_set = _pipeline->allocate_descriptor_set();

    _positions = make_vulkan_buffer(device, model.positions.get(), model.positions->size(), USAGE_VERTEX_BUFFER, PRIVATE_MEMORY);
    _texcoords = make_vulkan_buffer(device, model.texcoords.get(), model.texcoords->size(), USAGE_VERTEX_BUFFER, PRIVATE_MEMORY);
    _normals   = make_vulkan_buffer(device, model.normals.get(), model.normals->size(), USAGE_VERTEX_BUFFER, PRIVATE_MEMORY);
    _indices   = make_vulkan_buffer(device, model.indices.get(), model.indices->size(), USAGE_INDEX_BUFFER, PRIVATE_MEMORY);
    _primitives = model.primitives;

    _vs_config = make_vulkan_buffer(device, nullptr, _pipeline->vertex_shader()->resource_size(VS_CONFIG_NAME), USAGE_UNIFORM_BUFFER, SHARED_MEMORY);
    _fs_config = make_vulkan_buffer(device, nullptr, _pipeline->fragment_shader()->resource_size(FS_CONFIG_NAME), USAGE_UNIFORM_BUFFER, SHARED_MEMORY);
    _light     = make_vulkan_buffer(device, nullptr, _pipeline->fragment_shader()->resource_size(LIGHT_NAME), USAGE_UNIFORM_BUFFER, SHARED_MEMORY);

    VkDescriptorBufferInfo vs_config_info;
    vs_config_info.buffer = _vs_config->handle();
    vs_config_info.offset = 0;
    vs_config_info.range = _vs_config->size();

    VkDescriptorBufferInfo fs_config_info;
    fs_config_info.buffer = _fs_config->handle();
    fs_config_info.offset = 0;
    fs_config_info.range = _fs_config->size();

    VkDescriptorBufferInfo lights_info;
    lights_info.buffer = _light->handle();
    lights_info.offset = 0;
    lights_info.range = _light->size();

    VkWriteDescriptorSet default_ubo;
    default_ubo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    default_ubo.pNext = nullptr;
    default_ubo.dstSet = _descriptor_set;
    default_ubo.dstBinding = UINT32_MAX;
    default_ubo.dstArrayElement = 0;
    default_ubo.descriptorCount = 1;
    default_ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    default_ubo.pImageInfo = nullptr;
    default_ubo.pBufferInfo = nullptr;

    VkWriteDescriptorSet writes[3];
    writes[0] = default_ubo;
    writes[0].dstBinding = _pipeline->vertex_shader()->resource_binding(VS_CONFIG_NAME);
    writes[0].pBufferInfo = &vs_config_info;
    writes[1] = default_ubo;
    writes[1].dstBinding = _pipeline->fragment_shader()->resource_binding(FS_CONFIG_NAME);
    writes[1].pBufferInfo = &fs_config_info;
    writes[2] = default_ubo;
    writes[2].dstBinding = _pipeline->fragment_shader()->resource_binding(LIGHT_NAME);
    writes[2].pBufferInfo = &lights_info;

    vkUpdateDescriptorSets(_device->handle(), sizeof(writes) / sizeof(writes[0]), writes, 0, nullptr);
}

VkModelZHU::~VkModelZHU()
{
    if (_descriptor_set)
    {
        vkFreeDescriptorSets(_device->handle(), _device->descriptor_pool(), 1, &_descriptor_set);
        _descriptor_set = VK_NULL_HANDLE;
    }
}

void VkModelZHU::draw(VkCommandBuffer cmdbuf)
{
    VkDeviceSize zero_offset = 0;
    uint32_t attribute_position = _pipeline->vertex_shader()->resource_binding(ATTRIBUTE_POSITION_NAME);
    uint32_t attribute_texcoord = _pipeline->vertex_shader()->resource_binding(ATTRIBUTE_TEXCOORD_NAME);
    uint32_t attribute_normal   = _pipeline->vertex_shader()->resource_binding(ATTRIBUTE_NORMAL_NAME);

    vkCmdBindVertexBuffers(cmdbuf, attribute_position, 1, &_positions->handle(), &zero_offset);
    vkCmdBindVertexBuffers(cmdbuf, attribute_texcoord, 1, &_texcoords->handle(), &zero_offset);
    vkCmdBindVertexBuffers(cmdbuf, attribute_normal, 1, &_normals->handle(), &zero_offset);
    vkCmdBindIndexBuffer(cmdbuf, _indices->handle(), zero_offset, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->layout(), 0, 1, &_descriptor_set, 0, nullptr);
    vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->handle());
    
    for (const auto& [primitive, range] : _primitives)
    {
        vkCmdDrawIndexed(cmdbuf, range.count, 1, range.offset, 0, 0);
    }
}