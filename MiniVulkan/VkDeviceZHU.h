#pragma once

#include "VulkanConfig.h"
#include "VkPipelineZHU.h"

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

    void beginFrame(const MatrixMVP& mvp);
    void endFrame();
    void beginRenderPass(bool clearBackground);
    void endRenderPass();
    void waitIdle();
    void resize(int width, int height);

    GPUModel createGPUModel(const Model& src);
    GPUNurbs createGPUNurbs(const Nurbs& src);
    void deleteGPUModel(GPUModel& model);
    void deleteGPUNurbs(GPUNurbs& nurbs);
    void drawModel(const GPUModel& model);
    void drawNurbs(const GPUNurbs& nurbs);

private:
    void createVkInstance();
    void createDebugUtils();
    void createSurface();
    void choosePhysicalDevice();
    void chooseQueueFamilyIndex();
    void createLogicalDevice();
    void createCommandPool();
    void createDescriptorPool();
    void createSemaphorePool();
    void createFencePool();
    void chooseSwapchainProperties();
    void createSwapchain();
    void createDepthImage();
    void createRenderPasses();
    void createFramebuffers();
    void createPipelines();
    void deleteVkInstance();
    void deleteDebugUtils();
    void deleteSurface();
    void deleteLogicalDevice();
    void deleteCommandPool();
    void deleteDescriptorPool();
    void deleteSemaphorePool();
    void deleteFencePool();
    void deleteSwapchain();
    void deleteDepthImage();
    void deleteRenderPasses();
    void deleteFramebuffers();
    void deletePipelines();

    void acquireNextImage();
    void transitionImageLayout(VkImage image, VkImageLayout newLayout);
    void freeSemaphore(VkSemaphore semaphore);
    void freeFence(VkFence fence);

    GPUBuffer createGPUBuffer(const ARCData& data,
                              VkBufferUsageFlagBits usage,
                              VkMemoryPropertyFlags props);
    VkSemaphore allocateSemaphore();
    VkFence allocateFence();

private:
    // create info
    VkDeviceCreateInfoZHU _info;

    // vulkan instance data
    VkInstance _instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT _debugUtils = VK_NULL_HANDLE;

    // surface data
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkExtent2D _surfaceExtent;
    VkFormat _surfaceFormat = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR _surfaceColorspace;

    // physical device data
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkQueue  _queue = VK_NULL_HANDLE;
    uint32_t _queueFamilyIndex = UINT32_MAX;
    VkCommandPool _commandPool = VK_NULL_HANDLE;
    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
    std::deque<VkSemaphore> _semaphorePool;
    std::deque<VkFence> _fencePool;

    // swapchain data
    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    VkRenderPass   _renderPassLoad = VK_NULL_HANDLE;
    VkRenderPass   _renderPassClear = VK_NULL_HANDLE;
    VkImage        _depthImage = VK_NULL_HANDLE;
    VkImageView    _depthImageView = VK_NULL_HANDLE;
    VkDeviceMemory _depthImageMemory = VK_NULL_HANDLE;
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

    // resources
    std::map<VkImage, VkImageLayout> _imageLayouts;
    ARC<VkPipelineZHU> _pipeline2D;
    ARC<VkPipelineZHU> _pipeline3D;

    // current frame
    VkCommandBuffer _commandBuffer = VK_NULL_HANDLE;
    bool _submitAsync = false;
    MatrixMVP _mvp;
};