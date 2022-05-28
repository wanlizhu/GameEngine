#pragma once

#include "VulkanAPI.h"
#include "VkBufferZHU.h"
#include "VkPipelineZHU.h"

class VkModelZHU : public std::enable_shared_from_this<VkModelZHU>
{
public:
    VkModelZHU(VkDeviceZHU* device, const VkModelDB& model);
    VkModelZHU(const VkModelZHU&) = delete;
    VkModelZHU& operator=(const VkModelZHU&) = delete;
    virtual ~VkModelZHU();

    void draw(VkCommandBuffer cmdbuf);

private:
    VkDeviceZHU* _device = nullptr;
    std::shared_ptr<VkBufferZHU>  _positions;
    std::shared_ptr<VkBufferZHU>  _texcoords;
    std::shared_ptr<VkBufferZHU>  _normals;
    std::shared_ptr<VkBufferZHU>  _indices;
    std::unordered_map<VkPrimitiveTopology, IndexRange> _primitives;

    std::shared_ptr<VkBufferZHU> _vs_config;
    std::shared_ptr<VkBufferZHU> _fs_config;
    std::shared_ptr<VkBufferZHU> _light;
   
    std::shared_ptr<VkPipelineZHU> _pipeline;
    VkDescriptorSet _descriptor_set = VK_NULL_HANDLE;
};

std::shared_ptr<VkModelZHU> make_vulkan_model(VkDeviceZHU* device, const VkModelDB& model);