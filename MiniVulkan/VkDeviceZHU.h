#pragma once

#include "VulkanConfig.h"

struct VkDeviceCreateInfoZHU
{
    HWND window;
    int versionMajor;
    int versionMinor;
    std::vector<std::string> instanceExtensions;
    std::vector<std::string> deviceExtensions;
    VkPhysicalDeviceFeatures requiredFeatures;
};

class VkDeviceZHU : public std::enable_shared_from_this<VkDeviceZHU>
{
public:
    VkDeviceZHU(const VkDeviceCreateInfoZHU& info);
    VkDeviceZHU(const VkDeviceZHU&) = delete;
    VkDeviceZHU& operator=(const VkDeviceZHU&) = delete;
    virtual ~VkDeviceZHU();

    void beginFrame();
    void endFrame();
    void beginRenderPass(bool clearBackground);
    void endRenderPass();
    void waitIdle();
    void resize(int width, int height);

private:
    void createVkInstance();
    void createDebugUtils();
    void createSurface();
    void choosePhysicalDevice();
    void chooseQueueFamilyIndex();
    void createLogicalDevice();
    void createCommandPool();
    void chooseSwapchainProperties();
    void createSwapchain();
    void createDepthImage();
    void createRenderPass();
    void createFramebuffers();

    void acquireNextImage();
    void transitionImageLayout(VkImage image, VkImageLayout newLayout);

    VkFence allocateFence();
    VkSemaphore allocateSemaphore();
    void freeFence(VkFence& fence);
    void freeSemaphore(VkSemaphore& semaphore);

private:
    // create info
    VkDeviceCreateInfoZHU _info;

    // vulkan instance data
    VkInstance _instance = 0;
    VkDebugUtilsMessengerEXT _debugUtils = 0;

    // surface data
    VkSurfaceKHR _surface = 0;
    VkExtent2D _surfaceExtent;
    VkFormat _surfaceFormat = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR _surfaceColorspace;

    // physical device data
    VkPhysicalDevice _physicalDevice = 0;
    VkDevice _device = 0;
    VkQueue  _queue = 0;
    uint32_t _queueFamilyIndex = UINT32_MAX;
    VkCommandPool _commandPool = 0;

    // swapchain data
    VkSwapchainKHR _swapchain = 0;
    VkRenderPass   _renderPassLoad = 0;
    VkRenderPass   _renderPassClear = 0;
    VkImage        _depthImage = 0;
    VkImageView    _depthImageView = 0;
    VkDeviceMemory _depthImageMemory = 0;
    VkFormat       _depthFormat = VK_FORMAT_UNDEFINED;
    VkImageLayout  _depthImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    std::vector<VkImage> _backingImages;
    std::vector<VkImageView> _backingImageViews;
    std::vector<VkFramebuffer> _framebuffers;
    uint32_t _currentImageIndex = UINT32_MAX;

    // clear color and depth/stencil
    glm::vec3 _clearColor = glm::vec3(0.45);
    float _clearDepth = 1.0;
    int _clearStencil = 0;

    // resource state trackings
    std::map<VkImage, VkImageLayout> _imageLayouts;

    // current render pass
    VkCommandBuffer _commandBuffer = 0;
    bool _submitAsync = false;
};