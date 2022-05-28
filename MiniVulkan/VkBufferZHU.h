#pragma once

#include "VulkanAPI.h"
#include "BasicTools.h"

class VkBufferZHU : public std::enable_shared_from_this<VkBufferZHU>
{
public:
    VkBufferZHU(VkDeviceZHU* device, 
                const void* data, 
                size_t size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags mprops);
    VkBufferZHU(const VkBufferZHU&) = delete;
    VkBufferZHU& operator=(const VkBufferZHU&) = delete;
    virtual ~VkBufferZHU();

    void write(size_t offset, size_t size, const void* data);
    VkBuffer const& handle() const { return _buffer; }
    size_t size() const { return _size; }

private:
    VkDeviceZHU* _device = nullptr;
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;
    size_t _size = 0;
    VkMemoryPropertyFlags _memory_props;
};

std::shared_ptr<VkBufferZHU> make_vulkan_buffer(VkDeviceZHU* device,
                                                const void* data,
                                                size_t size,
                                                VkBufferUsageFlags usage,
                                                VkMemoryPropertyFlags mprops);


