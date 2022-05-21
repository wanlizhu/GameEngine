#include "VkDeviceZHU.h"
#include "VkCommonZHU.h"

#define ENABLE_WARNINGS 
#define SWAPCHAIN_IMAGE_COUNT 3

bool debugUtilsEnabled = false;
PFN_vkSetDebugUtilsObjectNameEXT    _vkSetDebugUtilsObjectNameEXT = nullptr;
PFN_vkCreateDebugUtilsMessengerEXT  _vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT _vkDestroyDebugUtilsMessengerEXT = nullptr;
PFN_vkCmdBeginDebugUtilsLabelEXT    _vkCmdBeginDebugUtilsLabelEXT = nullptr;
PFN_vkCmdEndDebugUtilsLabelEXT      _vkCmdEndDebugUtilsLabelEXT = nullptr;
PFN_vkCmdInsertDebugUtilsLabelEXT   _vkCmdInsertDebugUtilsLabelEXT = nullptr;

void loadDynamicFunctions(VkInstance instance)
{
    _vkSetDebugUtilsObjectNameEXT = VK_LOAD_FUNC(instance, vkSetDebugUtilsObjectNameEXT);
    _vkCreateDebugUtilsMessengerEXT = VK_LOAD_FUNC(instance, vkCreateDebugUtilsMessengerEXT);
    _vkDestroyDebugUtilsMessengerEXT = VK_LOAD_FUNC(instance, vkDestroyDebugUtilsMessengerEXT);
    _vkCmdBeginDebugUtilsLabelEXT = VK_LOAD_FUNC(instance, vkCmdBeginDebugUtilsLabelEXT);
    _vkCmdEndDebugUtilsLabelEXT = VK_LOAD_FUNC(instance, vkCmdEndDebugUtilsLabelEXT);
    _vkCmdInsertDebugUtilsLabelEXT = VK_LOAD_FUNC(instance, vkCmdInsertDebugUtilsLabelEXT);
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT        messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData)
{
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        printf("%s\n%s\n\n", callbackData->pMessageIdName, callbackData->pMessage);
    }

#ifdef ENABLE_WARNINGS
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        printf("%s\n%s\n\n", callbackData->pMessageIdName, callbackData->pMessage);
    }
#endif

    // Don't bail out, but keep going.
    return VK_FALSE;
}

void VkDeviceZHU::createVkInstance()
{
    assert(_instance == VK_NULL_HANDLE);

    checkVersion(_info.versionMajor, _info.versionMinor);

    std::vector<const char*> enabledLayers;
    std::vector<const char*> enabledExtensions;
    chooseInstanceLayersAndExtensions(_info, &enabledLayers, &enabledExtensions);

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.apiVersion = VK_MAKE_VERSION(_info.versionMajor, _info.versionMinor, 0);
    appInfo.applicationVersion = 0;
    appInfo.engineVersion = 0;
    appInfo.pApplicationName = "Playground";
    appInfo.pEngineName = "";

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = enabledLayers.size();
    createInfo.ppEnabledLayerNames = enabledLayers.data();
    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
    VK_CHECK(result);

    loadDynamicFunctions(_instance);
}

void VkDeviceZHU::deleteVkInstance()
{
    if (_instance)
    {
        vkDestroyInstance(_instance, nullptr);
        _instance = VK_NULL_HANDLE;
    }
}

void VkDeviceZHU::createDebugUtils()
{
    if (!findName(VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                  _info.instanceExtensions))
    {
        debugUtilsEnabled = false;
        return;
    }

    assert(_debugUtils == VK_NULL_HANDLE);

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugMessengerCallback;
    createInfo.pUserData = nullptr;

    VkResult result = _vkCreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugUtils);
    VK_CHECK(result);

    debugUtilsEnabled = true;
}

void VkDeviceZHU::deleteDebugUtils()
{
    if (_debugUtils)
    {
        _vkDestroyDebugUtilsMessengerEXT(_instance, _debugUtils, nullptr);
        _debugUtils = VK_NULL_HANDLE;
    }
}

void VkDeviceZHU::createSurface()
{
    assert(_surface == VK_NULL_HANDLE);

    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.flags = 0;
    surfaceInfo.hinstance = GetModuleHandle(NULL);
    surfaceInfo.hwnd = _info.window;

    VkResult result = vkCreateWin32SurfaceKHR(_instance, &surfaceInfo, nullptr, &_surface);
    VK_CHECK(result);
}

void VkDeviceZHU::deleteSurface()
{
    if (_surface)
    {
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        _surface = VK_NULL_HANDLE;
    }
}

void VkDeviceZHU::choosePhysicalDevice()
{
    uint32_t deviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    VK_CHECK(result);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    result = vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
    VK_CHECK(result);

    std::vector<VkPhysicalDevice> qualifiedDevices;

    for (int i = 0; i < deviceCount; i++)
    {
        VkPhysicalDevice current = devices[i];
        VkPhysicalDeviceFeatures features = {};

        vkGetPhysicalDeviceFeatures(current, &features);

        uint32_t extensionCount = 0;
        result = vkEnumerateDeviceExtensionProperties(current, nullptr, &extensionCount, nullptr);
        VK_CHECK(result);

        std::vector<VkExtensionProperties> extensions;
        extensions.resize(extensionCount);
        result = vkEnumerateDeviceExtensionProperties(current, nullptr, &extensionCount, extensions.data());
        VK_CHECK(result);

        if (!contains(extensions, _info.deviceExtensions))
            continue;

        if (!contains(features, _info.requiredFeatures))
            continue;

        qualifiedDevices.push_back(current);
    }

    for (int i = 1; i < qualifiedDevices.size(); i++)
    {
        if (comparePhysicalDevice(qualifiedDevices[0], qualifiedDevices[i]))
        {
            std::swap(qualifiedDevices[0], qualifiedDevices[i]);
        }
    }

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(qualifiedDevices[0], &props);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(qualifiedDevices[0], &features);

    _physicalDevice = qualifiedDevices[0];

    size_t total = getMemorySizeTotal(_physicalDevice);

    printf("__________\n");
    printf("  GPU Name: %s\n", props.deviceName);
    printf("    GPU ID: %d\n", props.deviceID);
    printf("GPU Memory: %s\n", sizeFormat(total));
    printf("\n");
}

void VkDeviceZHU::chooseQueueFamilyIndex()
{
    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &familyCount, nullptr);

    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &familyCount, families.data());
    
    uint32_t queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT;

    for (int i = 0; i < familyCount; i++)
    {
        if (_queueFamilyIndex != UINT32_MAX)
        {
            break;
        }

        if (families[i].queueFlags & queueFlags)
        {
            VkBool32 supportsPresent = VK_FALSE;
            VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, 0, _surface, &supportsPresent);
            VK_CHECK(result);

            if (supportsPresent)
            {
                _queueFamilyIndex = i;
            }
        }
    }
}

void VkDeviceZHU::createLogicalDevice()
{
    assert(_surface != VK_NULL_HANDLE);
    assert(_physicalDevice != VK_NULL_HANDLE);
    assert(_device == VK_NULL_HANDLE);

    std::vector<const char*> enabledExtensions;
    chooseDeviceExtensions(_info, _physicalDevice, &enabledExtensions);

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    addDeviceQueueCreateInfos(_info, _queueFamilyIndex, &queueInfos);

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pEnabledFeatures = &_info.requiredFeatures;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();
    createInfo.queueCreateInfoCount = queueInfos.size();
    createInfo.pQueueCreateInfos = queueInfos.data();

    VkResult result = vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device);
    VK_CHECK(result);

    vkGetDeviceQueue(_device, _queueFamilyIndex, 0, &_queue);
}

void VkDeviceZHU::deleteLogicalDevice()
{
    if (_device)
    {
        vkDestroyDevice(_device, nullptr);
        _device = VK_NULL_HANDLE;
    }
}

void VkDeviceZHU::createCommandPool()
{
    assert(_commandPool == VK_NULL_HANDLE);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.flags = 0;
    poolInfo.queueFamilyIndex = _queueFamilyIndex;

    VkResult result = vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
    VK_CHECK(result);
}

void VkDeviceZHU::deleteCommandPool()
{
    if (_commandPool)
    {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
        _commandPool = VK_NULL_HANDLE;
    }
}

void VkDeviceZHU::createDescriptorPool()
{
    assert(_descriptorPool == VK_NULL_HANDLE);

    std::vector<VkDescriptorPoolSize> sizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 32},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 32},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  32},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,  32},
    };

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.maxSets = 32;
    createInfo.poolSizeCount = sizes.size();
    createInfo.pPoolSizes = sizes.data();
    
    VkResult result = vkCreateDescriptorPool(_device, &createInfo, nullptr, &_descriptorPool);
    VK_CHECK(result);
}

void VkDeviceZHU::deleteDescriptorPool()
{
    if (_descriptorPool)
    {
        vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
        _descriptorPool = VK_NULL_HANDLE;
    }
}

void VkDeviceZHU::createSemaphorePool()
{
    assert(_semaphorePool.empty());

    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    for (int i = 0; i < SWAPCHAIN_IMAGE_COUNT * 2; i++)
    {
        VkSemaphore semaphore = VK_NULL_HANDLE;
        VkResult result = vkCreateSemaphore(_device, &createInfo, nullptr, &semaphore);
        VK_CHECK(result);

        _semaphorePool.push_back(semaphore);
    }
}

void VkDeviceZHU::deleteSemaphorePool()
{
    for (const auto& semaphore : _semaphorePool)
    {
        vkDestroySemaphore(_device, semaphore, nullptr);
    }

    _semaphorePool.clear();
}

void VkDeviceZHU::createFencePool()
{
    assert(_fencePool.empty());

    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    for (int i = 0; i < SWAPCHAIN_IMAGE_COUNT * 2; i++)
    {
        VkFence fence = VK_NULL_HANDLE;
        VkResult result = vkCreateFence(_device, &createInfo, nullptr, &fence);
        VK_CHECK(result);

        _fencePool.push_back(fence);
    }
}

void VkDeviceZHU::deleteFencePool()
{
    for (const auto& fence : _fencePool)
    {
        vkDestroyFence(_device, fence, nullptr);
    }

    _fencePool.clear();
}

void VkDeviceZHU::createDepthImage()
{
    assert(_surface != VK_NULL_HANDLE);
    assert(_depthImage == VK_NULL_HANDLE);

    if (_depthFormat == VK_FORMAT_UNDEFINED)
        return;

    assert(false);
}

void VkDeviceZHU::deleteDepthImage()
{
    if (_depthImage)
    {
        vkDestroyImageView(_device, _depthImageView, nullptr);
        vkDestroyImage(_device, _depthImage, nullptr);
        vkFreeMemory(_device, _depthImageMemory, nullptr);

        _depthImage = VK_NULL_HANDLE;
        _depthImageView = VK_NULL_HANDLE;
        _depthImageMemory = VK_NULL_HANDLE;
    }
}

void VkDeviceZHU::createRenderPasses()
{
    assert(_renderPassLoad);
    assert(_renderPassClear);

    _renderPassLoad = createVkRenderPass(_device, 
                                         _surfaceFormat,
                                         _depthFormat,
                                         VK_ATTACHMENT_LOAD_OP_LOAD);
    _renderPassClear = createVkRenderPass(_device,
                                          _surfaceFormat,
                                          _depthFormat,
                                          VK_ATTACHMENT_LOAD_OP_CLEAR);
}

void VkDeviceZHU::deleteRenderPasses()
{
    if (_renderPassLoad)
    {
        vkDestroyRenderPass(_device, _renderPassLoad, nullptr);
        _renderPassLoad = VK_NULL_HANDLE;
    }

    if (_renderPassClear)
    {
        vkDestroyRenderPass(_device, _renderPassClear, nullptr);
        _renderPassClear = VK_NULL_HANDLE;
    }
}

void VkDeviceZHU::createFramebuffers()
{
    assert(_swapchain != VK_NULL_HANDLE);
    assert(_framebuffers.empty());

    _framebuffers.resize(_backingImages.size());

    for (int i = 0; i < _backingImageViews.size(); i++)
    {
        _framebuffers[i] = createVkFramebuffer(_device,
                                               _backingImageViews[i],
                                               _depthImageView,
                                               _surfaceExtent,
                                               _renderPassLoad,
                                               VK_ATTACHMENT_LOAD_OP_LOAD);
    }
}

void VkDeviceZHU::deleteFramebuffers()
{
    for (auto& framebuffer : _framebuffers)
    {
        if (framebuffer)
        {
            vkDestroyFramebuffer(_device, framebuffer, nullptr);
            framebuffer = VK_NULL_HANDLE;
        }
    }

    _framebuffers.clear();
}

void VkDeviceZHU::createPipelines()
{
    assert(_swapchain);
    
    VkRect2D screenRect;
    screenRect.offset = { 0, 0 };
    screenRect.extent = _surfaceExtent;
    
    VkPipelineCreateInfoZHU info3D;
    info3D.device = _device;
    info3D.addShaderModule("StandardSurface.vert.glsl");
    info3D.addShaderModule("StandardSurface.frag.glsl");
    info3D.setVertexAttribute(0, VK_FORMAT_R32G32B32_SFLOAT);
    info3D.setVertexAttribute(1, VK_FORMAT_R32G32_SFLOAT);
    info3D.setVertexAttribute(2, VK_FORMAT_R32G32B32_SFLOAT);
    info3D.setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    info3D.setRasterState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    info3D.setColorBlendState(0, true);
    info3D.setDepthState(false, false);
    info3D.setStencilState(false, false);
    info3D.setViewportState(screenRect, screenRect);
    info3D.setRenderPass(_renderPassLoad, 0);

    _pipeline3D = std::make_shared<VkPipelineZHU>(info3D);
}

void VkDeviceZHU::deletePipelines()
{
    _pipeline2D = nullptr;
    _pipeline3D = nullptr;
}

GPUBuffer VkDeviceZHU::createGPUBuffer(const ARCData& data,
                                       VkBufferUsageFlagBits usage,
                                       VkMemoryPropertyFlags props)
{
    VkBuffer buffer = createVkBuffer(_device, data->size(), usage);
    VkDeviceMemory memory = createAndBindBufferMemory(_device, _physicalDevice, buffer, props);

    if (props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        updateHostVisibleBuffer(_device,
                                memory,
                                0,
                                data->size(),
                                data->data());
    }
    else
    {
        updateHostInvisibleBuffer(_device, 
                                  _physicalDevice, 
                                  _queue,
                                  _commandPool,
                                  buffer,
                                  0, 
                                  data->size(),
                                  data->data());
    }

    return GPUBuffer{ buffer, memory };
}

void VkDeviceZHU::chooseSwapchainProperties()
{
    RECT rect;
    GetClientRect(_info.window, &rect);

    _surfaceExtent = VkExtent2D{
        uint32_t(rect.right - rect.left),
        uint32_t(rect.bottom - rect.top),
    };

    findFormatAndColorSpace(_physicalDevice,
                            _surface,
                            &_surfaceFormat,
                            &_surfaceColorspace);
}

void VkDeviceZHU::createSwapchain()
{
    assert(_surface != VK_NULL_HANDLE);
    assert(_device != VK_NULL_HANDLE);

    if (_swapchain)
    {
        deleteSwapchain();
        chooseSwapchainProperties();
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.surface = _surface;
    createInfo.minImageCount = SWAPCHAIN_IMAGE_COUNT;
    createInfo.imageFormat = _surfaceFormat;
    createInfo.imageColorSpace = _surfaceColorspace;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageExtent = _surfaceExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = &_queueFamilyIndex;
    createInfo.clipped = VK_FALSE;
    createInfo.compositeAlpha = getCompositeAlphaFlag(_physicalDevice, _surface);
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.presentMode = getPresentMode(_physicalDevice, _surface);
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapchain);
    VK_CHECK(result);

    _renderPassLoad  = createVkRenderPass(_device, _surfaceFormat, _depthFormat, VK_ATTACHMENT_LOAD_OP_LOAD);
    _renderPassClear = createVkRenderPass(_device, _surfaceFormat, _depthFormat, VK_ATTACHMENT_LOAD_OP_CLEAR);

    uint32_t imageCount = 0;
    result = vkGetSwapchainImagesKHR(_device, _swapchain, &imageCount, nullptr);
    VK_CHECK(result);

    _backingImages.resize(imageCount);
    result = vkGetSwapchainImagesKHR(_device, _swapchain, &imageCount, _backingImages.data());
    VK_CHECK(result);

    if (_depthFormat != VK_FORMAT_UNDEFINED)
    {
        assert(isDepthFormat(_depthFormat));
        assert(false);
    }

    _backingImageViews.resize(imageCount);

    for (int i = 0; i < imageCount; i++)
    {
        _backingImageViews[i] = createVkImageView(_device, _backingImages[i], _surfaceFormat);
    }
}

void VkDeviceZHU::deleteSwapchain()
{
    for (auto& framebuffer : _framebuffers)
    {
        vkDestroyFramebuffer(_device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    for (auto& view : _backingImageViews)
    {
        vkDestroyImageView(_device, view, nullptr);
        view = VK_NULL_HANDLE;
    }

    if (_depthImage)
    {
        vkFreeMemory(_device, _depthImageMemory, nullptr);
        vkDestroyImageView(_device, _depthImageView, nullptr);
        vkDestroyImage(_device, _depthImage, nullptr);
        _depthImage = VK_NULL_HANDLE;
        _depthImageView = VK_NULL_HANDLE;
        _depthImageMemory = VK_NULL_HANDLE;
    }

    vkDestroyRenderPass(_device, _renderPassLoad, nullptr);
    vkDestroyRenderPass(_device, _renderPassClear, nullptr);
    _renderPassLoad = VK_NULL_HANDLE;
    _renderPassClear = VK_NULL_HANDLE;

    vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    _swapchain = VK_NULL_HANDLE;

    _backingImages.clear();
    _backingImageViews.clear();
    _framebuffers.clear();
    _currentImageIndex = UINT32_MAX;
}

VkDeviceZHU::VkDeviceZHU(const VkDeviceCreateInfoZHU& info)
    : _info(info)
{
    createVkInstance();
    createDebugUtils();
    createSurface();
    choosePhysicalDevice();
    chooseQueueFamilyIndex();
    createLogicalDevice();
    createCommandPool();
    createDescriptorPool();
    createSemaphorePool();
    createFencePool();
    chooseSwapchainProperties();
    createSwapchain();
    createDepthImage();
    createRenderPasses();
    createFramebuffers();
    createPipelines();

    _mvp.modelMatrix = glm::mat4(1.0);
    _mvp.viewMatrix = glm::mat4(1.0);
    _mvp.projectionMatrix = glm::mat4(1.0);
}

VkDeviceZHU::~VkDeviceZHU()
{
    deletePipelines();
    deleteFramebuffers();
    deleteRenderPasses();
    deleteDepthImage();
    deleteSwapchain();
    deleteFencePool();
    deleteSemaphorePool();
    deleteDescriptorPool();
    deleteCommandPool();
    deleteLogicalDevice();
    deleteSurface();
    deleteDebugUtils();
    deleteVkInstance();
}

void VkDeviceZHU::beginRenderPass(bool clearBackground)
{
    assert(_commandBuffer == VK_NULL_HANDLE);
    assert(_currentImageIndex != UINT32_MAX);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _commandPool;
    allocInfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(_device, &allocInfo, &_commandBuffer);
    VK_CHECK(result);

    VkCommandBufferBeginInfo cmdBeginInfo = {};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.pNext = nullptr;
    cmdBeginInfo.flags = 0;
    cmdBeginInfo.pInheritanceInfo = nullptr;

    result = vkBeginCommandBuffer(_commandBuffer, &cmdBeginInfo);
    VK_CHECK(result);

    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = _clearColor[0];
    clearValues[0].color.float32[1] = _clearColor[1];
    clearValues[0].color.float32[2] = _clearColor[2];
    clearValues[0].color.float32[3] = 1.0;
    clearValues[1].depthStencil.depth = _clearDepth;
    clearValues[1].depthStencil.stencil = _clearStencil;

    VkRenderPassBeginInfo passBeginInfo = {};
    passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    passBeginInfo.pNext = nullptr;
    passBeginInfo.renderPass = clearBackground ? _renderPassClear : _renderPassLoad;
    passBeginInfo.framebuffer = _framebuffers[_currentImageIndex];
    passBeginInfo.renderArea.offset = VkOffset2D{ 0, 0 };
    passBeginInfo.renderArea.extent = _surfaceExtent;
    passBeginInfo.clearValueCount = clearBackground ? (_depthImage ? 2 : 1) : 0;
    passBeginInfo.pClearValues = clearBackground ? clearValues : nullptr;

    vkCmdBeginRenderPass(_commandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VkDeviceZHU::endRenderPass()
{
    assert(_commandBuffer);

    vkCmdEndRenderPass(_commandBuffer);
    vkEndCommandBuffer(_commandBuffer);

    VkPipelineStageFlags stageWait = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSemaphore semaphoreWait = VK_NULL_HANDLE;
    VkSemaphore semaphoreSignal = VK_NULL_HANDLE;

    if (_submitAsync)
    {
        assert(false);
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;
    submitInfo.waitSemaphoreCount = _submitAsync ? 1 : 0;
    submitInfo.pWaitSemaphores = &semaphoreWait;
    submitInfo.pWaitDstStageMask = &stageWait;
    submitInfo.signalSemaphoreCount = _submitAsync ? 1 : 0;
    submitInfo.pSignalSemaphores = &semaphoreSignal;

    VkFence fence = _submitAsync ? VK_NULL_HANDLE : allocateFence();
    VkResult result = vkQueueSubmit(_queue, 1, &submitInfo, fence);
    VK_CHECK(result);

    if (fence)
    {
        result = vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX);
        VK_CHECK(result);
        freeFence(fence);
    }

    _commandBuffer = VK_NULL_HANDLE;
}

void VkDeviceZHU::beginFrame(const MatrixMVP& mvp)
{
    _mvp = mvp;
    acquireNextImage();
}

void VkDeviceZHU::endFrame()
{
    VkSemaphore semaphoreWait = VK_NULL_HANDLE;

    if (_submitAsync)
    {
        assert(false);
    }
    
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = _submitAsync ? 1 : 0;
    presentInfo.pWaitSemaphores = &semaphoreWait;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchain;
    presentInfo.pImageIndices = &_currentImageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(_queue, &presentInfo);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        createSwapchain();
        createDepthImage();
        createRenderPasses();
        createFramebuffers();
    }
    else
    {
        VK_CHECK(result);
    }
}

void VkDeviceZHU::waitIdle()
{
    VkResult result = vkDeviceWaitIdle(_device);
    VK_CHECK(result);
}

void VkDeviceZHU::resize(int width, int height)
{
    if (_surfaceExtent.width != width ||
        _surfaceExtent.height != height)
    {
        createSwapchain();
        createDepthImage();
        createRenderPasses();
        createFramebuffers();
    }
}

void VkDeviceZHU::acquireNextImage()
{
    static bool _acquireAgain = false;
    VkSemaphore semaphoreAcquire = VK_NULL_HANDLE;
    VkFence fenceAcquire = VK_NULL_HANDLE;
    uint32_t indexAcquired = UINT32_MAX;

    if (_submitAsync)
    {
        assert(false);
    }
    else
    {
        fenceAcquire = allocateFence();
    }

    VkResult result = vkAcquireNextImageKHR(_device, 
                                            _swapchain,
                                            UINT64_MAX, 
                                            semaphoreAcquire,
                                            fenceAcquire,
                                            &indexAcquired);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        if (_acquireAgain)
        {
            _currentImageIndex = UINT32_MAX;
        }
        else
        {
            _acquireAgain = true;
            createSwapchain();
            createDepthImage();
            createRenderPasses();
            createFramebuffers();
            acquireNextImage();
            _acquireAgain = false;
        }
    }
    else
    {
        VK_CHECK(result);

        if (_submitAsync)
        {
            assert(false);
        }
        else
        {
            VkResult result = vkWaitForFences(_device, 1, &fenceAcquire, VK_TRUE, UINT64_MAX);
            VK_CHECK(result);
            freeFence(fenceAcquire);
        }

        _currentImageIndex = indexAcquired;
        assert(_currentImageIndex != UINT32_MAX);
    }
}

void VkDeviceZHU::transitionImageLayout(VkImage image, VkImageLayout newLayout)
{
    VkImageSubresourceRange subresource;
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.baseArrayLayer = 0;
    subresource.baseMipLevel = 0;
    subresource.layerCount = VK_REMAINING_ARRAY_LAYERS;
    subresource.levelCount = VK_REMAINING_MIP_LEVELS;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.image = image;
    barrier.subresourceRange = subresource;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.oldLayout = _imageLayouts[image];
    barrier.newLayout = newLayout;
    barrier.srcAccessMask = getAccessFlagsForImageLayout(barrier.oldLayout);
    barrier.dstAccessMask = getAccessFlagsForImageLayout(barrier.newLayout);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    vkCmdPipelineBarrier(_commandBuffer,
                         getStageFlagsForImageLayout(barrier.oldLayout),
                         getStageFlagsForImageLayout(barrier.newLayout),
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    _imageLayouts[image] = newLayout;
}

VkSemaphore VkDeviceZHU::allocateSemaphore()
{
    VkSemaphore semaphore = _semaphorePool.front();
    _semaphorePool.pop_front();

    return semaphore;
}

void VkDeviceZHU::freeSemaphore(VkSemaphore semaphore)
{
    _semaphorePool.push_back(semaphore);
}

VkFence VkDeviceZHU::allocateFence()
{
    VkFence fence = _fencePool.front();
    _fencePool.pop_front();

    VkResult result = vkResetFences(_device, 1, &fence);
    VK_CHECK(result);

    return fence;
}

void VkDeviceZHU::freeFence(VkFence fence)
{
    _fencePool.push_back(fence);
}

GPUModel VkDeviceZHU::createGPUModel(const Model& src)
{
    GPUModel model;
    model.positions = createGPUBuffer(src.positions, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    model.texcoords = createGPUBuffer(src.texcoords, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    model.normals = createGPUBuffer(src.normals, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    model.indices = createGPUBuffer(src.indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    model.meshes = src.meshes;
    assert(false);

    return model;
}

GPUNurbs VkDeviceZHU::createGPUNurbs(const Nurbs& src)
{
    return {};
}

void VkDeviceZHU::deleteGPUModel(GPUModel& model)
{}

void VkDeviceZHU::deleteGPUNurbs(GPUNurbs& nurbs)
{}

void VkDeviceZHU::drawModel(const GPUModel& model)
{
    
}

void VkDeviceZHU::drawNurbs(const GPUNurbs& nurbs)
{}