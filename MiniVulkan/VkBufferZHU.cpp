#include "VkBufferZHU.h"
#include "VkDeviceZHU.h"

VkBufferZHU::VkBufferZHU(VkDeviceZHU* device,
                         const void* data,
                         size_t size,
                         VkBufferUsageFlags usage,
                         VkMemoryPropertyFlags mprops)
    : _device(device)
    , _memory_props(mprops)
{
    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.size = size;
    info.usage = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;

    VkResult result = vkCreateBuffer(device->handle(), &info, nullptr, &_buffer);
    VK_CHECK(result);

    bool host_visible = _memory_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    uint32_t memory_type_index = host_visible ? _device->shared_memory_type_index() : _device->private_memory_type_index();

    VkMemoryRequirements requirements = {};
    vkGetBufferMemoryRequirements(device->handle(), _buffer, &requirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = memory_type_index;
    allocInfo.allocationSize = requirements.size;

    VkResult result = vkAllocateMemory(device->handle(), &allocInfo, nullptr, &_memory);
    VK_CHECK(result);

    result = vkBindBufferMemory(device->handle(), _buffer, _memory, 0);
    VK_CHECK(result);

    write(0, size, data);
}

VkBufferZHU::~VkBufferZHU()
{
    if (_buffer)
    {
        vkDestroyBuffer(_device->handle(), _buffer, nullptr);
        _buffer = VK_NULL_HANDLE;
    }

    if (_memory)
    {
        vkFreeMemory(_device->handle(), _memory, nullptr);
        _memory = VK_NULL_HANDLE;
    }
}

void VkBufferZHU::write(size_t offset, size_t size, const void* data)
{
    if (_memory_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        void* mapped = nullptr;
        VkResult result = vkMapMemory(_device->handle(), _memory, offset, size, 0, &mapped);
        VK_CHECK(result);

        memcpy(mapped, data, size);
        vkUnmapMemory(_device->handle(), _memory);
    }
    else
    {
        VkBufferCopy region = {};
        region.srcOffset = 0;
        region.dstOffset = 0;
        region.size = size;

        auto staging = make_vulkan_buffer(_device, data, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, SHARED_MEMORY);

        VkCommandBuffer cmdbuf = _device->open_command_buffer();
        vkCmdCopyBuffer(cmdbuf, staging->_buffer, _buffer, 1, &region);
        _device->close_command_buffer(cmdbuf, true);
    }
}

std::shared_ptr<VkBufferZHU> make_vulkan_buffer(VkDeviceZHU* device,
                                                const void* data,
                                                size_t size,
                                                VkBufferUsageFlags usage,
                                                VkMemoryPropertyFlags mprops)
{
    return std::make_shared<VkBufferZHU>(device, data, size, usage, mprops);
}
