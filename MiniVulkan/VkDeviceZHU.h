#pragma once

#include "VulkanAPI.h"
#include "VkPipelineZHU.h"

struct VkDeviceCreateInfoZHU
{
    HWND window;
    int version_major;
    int version_minor;
    std::vector<std::string> instance_extensions;
    std::vector<std::string> device_extensions;
    VkPhysicalDeviceFeatures required_features;
};

class VkDeviceZHU : public std::enable_shared_from_this<VkDeviceZHU>
{
public:
    VkDeviceZHU(const VkDeviceCreateInfoZHU& info);
    VkDeviceZHU(const VkDeviceZHU&) = delete;
    VkDeviceZHU& operator=(const VkDeviceZHU&) = delete;
    virtual ~VkDeviceZHU();

    void begin_render_pass(VkCommandBuffer cmdbuf, bool clear);
    void end_render_pass(VkCommandBuffer cmdbuf);
    void wait_idle();
    void resize(int width, int height);
    void transition_image_layout(VkImage image, VkImageLayout new_layout);

    VkCommandBuffer open_command_buffer();
    void close_command_buffer(VkCommandBuffer, bool wait);

    VkSemaphore allocate_semaphore();
    VkFence allocate_fence();
    void free_semaphore(VkSemaphore semaphore);
    void free_fence(VkFence fence);

    VkDevice handle() const { return _device; }
    VkDescriptorPool descriptor_pool() const { return _descriptor_pool; }
    uint32_t shared_memory_type_index() const { return _shared_memory_type_index; }
    uint32_t private_memory_type_index() const { return _private_memory_type_index; }


private:
    void create_vulkan_instance();
    void create_debug_utils();
    void create_surface();
    void choose_physical_device();
    void create_logical_device();
    void create_command_pool();
    void create_descriptor_pool();
    void create_semaphore_pool();
    void create_fence_pool();
    void choose_swapchain_properties();
    void create_swapchain();
    void create_swapchain_depth_image();
    void create_swapchain_render_pass();
    void create_swapchain_framebuffer();
    void acquire_next_image();
    void present_this_image();

    void delete_vulkan_instance();
    void delete_debug_utils();
    void delete_surface();
    void delete_logical_device();
    void delete_command_pool();
    void delete_descriptor_pool();
    void delete_semaphore_pool();
    void delete_fence_pool();
    void delete_swapchain();
    void delete_swapchain_depth_image();
    void delete_swapchain_render_pass();
    void delete_swapchain_framebuffer();

private:
    // create info
    VkDeviceCreateInfoZHU _info;

    // vulkan instance data
    VkInstance _instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT _messenger = VK_NULL_HANDLE;

    // surface data
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkExtent2D _surface_extent;
    VkFormat _surface_format = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR _surface_colorspace;

    // physical device data
    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkQueue  _queue = VK_NULL_HANDLE;
    uint32_t _queue_family_index = UINT32_MAX;
    uint32_t _shared_memory_type_index = UINT32_MAX;
    uint32_t _private_memory_type_index = UINT32_MAX;
    VkCommandPool _command_pool = VK_NULL_HANDLE;
    VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;
    std::deque<VkSemaphore> _semaphore_pool;
    std::deque<VkFence> _fence_pool;

    // swapchain data
    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    VkRenderPass   _swapchain_render_pass = VK_NULL_HANDLE;
    VkImage        _swapchain_depth_image = VK_NULL_HANDLE;
    VkImageView    _swapchain_depth_image_view = VK_NULL_HANDLE;
    VkDeviceMemory _swapchain_depth_image_memory = VK_NULL_HANDLE;
    VkFormat       _swapchain_depth_format = VK_FORMAT_UNDEFINED;
    std::vector<VkImage>       _swapchain_images;
    std::vector<VkImageView>   _swapchain_image_views;
    std::vector<VkFramebuffer> _swapchain_framebuffers;
    uint32_t _current_image_index = UINT32_MAX;

    // clear color and depth/stencil
    glm::vec3 _clear_color = glm::vec3(0.45);
    float _clear_depth = 1.0;
    int _clear_stencil = 0;
};

std::shared_ptr<VkDeviceZHU> make_vulkan_device(HWND window);