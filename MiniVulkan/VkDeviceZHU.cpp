#include "VkDeviceZHU.h"

#define ENABLE_WARNINGS 

bool debugUtilsEnabled = false;
PFN_vkSetDebugUtilsObjectNameEXT    _vkSetDebugUtilsObjectNameEXT = nullptr;
PFN_vkCreateDebugUtilsMessengerEXT  _vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT _vkDestroyDebugUtilsMessengerEXT = nullptr;
PFN_vkCmdBeginDebugUtilsLabelEXT    _vkCmdBeginDebugUtilsLabelEXT = nullptr;
PFN_vkCmdEndDebugUtilsLabelEXT      _vkCmdEndDebugUtilsLabelEXT = nullptr;
PFN_vkCmdInsertDebugUtilsLabelEXT   _vkCmdInsertDebugUtilsLabelEXT = nullptr;

VkAccessFlags getAccessFlagsForImageLayout(VkImageLayout layout)
{
    switch (layout)
    {
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            return VK_ACCESS_HOST_WRITE_BIT;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return VK_ACCESS_TRANSFER_WRITE_BIT;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return VK_ACCESS_TRANSFER_READ_BIT;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return VK_ACCESS_SHADER_READ_BIT;

        default:
            return VkAccessFlags();
    }
}

VkPipelineStageFlags getStageFlagsForImageLayout(VkImageLayout layout)
{
    switch (layout)
    {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return VK_PIPELINE_STAGE_TRANSFER_BIT;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            //return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;  // We do this to allow queue other than graphic
            return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            //return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;  // We do this to allow queue other than graphic
            return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            return VK_PIPELINE_STAGE_HOST_BIT;

        case VK_IMAGE_LAYOUT_UNDEFINED:
            return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        default:
            return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
}

bool findLayerName(const char* layerName,
                   std::vector<VkLayerProperties> const& layers)
{
    auto it = std::find_if(layers.begin(),
                           layers.end(),
                           [&](VkLayerProperties const& props) {
        return strcmp(props.layerName, layerName) == 0;
    });

    return it != layers.end();
}

bool findExtensionName(const char* extensionName,
                       std::vector<VkExtensionProperties> const& extensions)
{
    auto it = std::find_if(extensions.begin(),
                           extensions.end(),
                           [&](VkExtensionProperties const& props) {
        return strcmp(props.extensionName, extensionName) == 0;
    });

    return it != extensions.end();
}

bool findName(const char* name,
              std::vector<std::string> const& list)
{
    auto it = std::find_if(list.begin(),
                           list.end(),
                           [&](const auto& element) {
        return strcmp(element.c_str(), name) == 0;
    });

    return it != list.end();
}

void chooseInstanceLayersAndExtensions(VkDeviceCreateInfoZHU& info,
                                       std::vector<const char*>* layerNamesP,
                                       std::vector<const char*>* extensionNamesP)
{
    uint32_t layerCount = 0;
    VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    VK_CHECK(result);

    std::vector<VkLayerProperties> layers(layerCount);
    result = vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    VK_CHECK(result);

    //for (int i = 0; i < info.validationLayers.size(); i++)
    //{
    //    if (!findLayerName(info.validationLayers[i], layers))
    //    {
    //        printf("Layer %s is not supported\n", info.validationLayers[i]);
    //        continue;
    //    }
    //
    //    layerNamesP->push_back(info.validationLayers[i]);
    //}

    static std::list<std::string> cache;
    for (const auto& layer : layers)
    {
        if (!strstr(layer.layerName, "api_dump") &&
            !strstr(layer.layerName, "capture") &&
            !strstr(layer.layerName, "gfxreconstruct") &&
            !strstr(layer.layerName, "profiles"))
        {
            cache.push_back(layer.layerName);
            layerNamesP->push_back(cache.back().c_str());
        }
    }

    uint32_t extensionCount = 0;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    VK_CHECK(result);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    VK_CHECK(result);

    std::vector<std::string> extensionNames;

    for (int i = 0; i < info.instanceExtensions.size(); i++)
    {
        if (!findExtensionName(info.instanceExtensions[i].c_str(), extensions))
        {
            printf("Extension %s is not supported\n", info.instanceExtensions[i]);
            continue;
        }

        extensionNames.push_back(info.instanceExtensions[i].c_str());
    }

    info.instanceExtensions = extensionNames;

    for (const auto& extension : info.instanceExtensions)
    {
        extensionNamesP->push_back(extension.c_str());
    }
}

bool checkVersion(int major, int minor)
{
    uint32_t version = 0;
    VkResult result = vkEnumerateInstanceVersion(&version);
    VK_CHECK(result);

    printf("_______________\n");
    printf("Vulkan Version:\n");
    printf(" - Available: %d.%d.%d\n", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
    printf(" -  Requires: %d.%d.%d\n", major, minor, 0);
    printf("\n");

    return (VK_VERSION_MAJOR(version) >= major) && (VK_VERSION_MINOR(version) >= minor);
}

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

void VkDeviceZHU::createDebugUtils()
{
    if (!findName(VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                  _info.instanceExtensions))
    {
        debugUtilsEnabled = false;
        return;
    }

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

const char* getFormatName(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R8G8B8A8_UNORM: return "RGBA8_UNORM";
        case VK_FORMAT_R8G8B8A8_SRGB: return "RGBA8_SRGB";
        case VK_FORMAT_R32G32B32A32_SFLOAT: return "RGBA32_FLOAT";
        case VK_FORMAT_R32G32_SFLOAT: return "RG32_FLOAT";
        case VK_FORMAT_R32G32_UINT: return "RG32_UINT";
        case VK_FORMAT_R32_SFLOAT: return "R32_FLOAT";
        case VK_FORMAT_R32_UINT: return "R32_UINT";
        case VK_FORMAT_D32_SFLOAT: return "D32_FLOAT";
        case VK_FORMAT_D32_SFLOAT_S8_UINT: return "D32_FLOAT_S8_UINT";
        case VK_FORMAT_UNDEFINED: return "UNDEFINED";
        default: 
        {
            static std::string cache;
            cache = std::to_string(format);
            return cache.c_str();
        }
    }
}

void findFormatAndColorSpace(VkPhysicalDevice physicalDevice,
                             VkSurfaceKHR surface,
                             VkFormat* formatP,
                             VkColorSpaceKHR* colorSpaceP)
{
    uint32_t formatCount;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    VK_CHECK(result);

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
    VK_CHECK(result);

    if (formatCount == 0)
    {
        *formatP = (*formatP == VK_FORMAT_UNDEFINED) ? VK_FORMAT_R8G8B8A8_UNORM : *formatP;
        *colorSpaceP = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        return;
    }

    if (*formatP == VK_FORMAT_UNDEFINED)
    {
        *formatP = surfaceFormats[0].format;
        *colorSpaceP = surfaceFormats[0].colorSpace;
        return;
    }

    for (int i = 0; i < formatCount; i++)
    {
        if (surfaceFormats[i].format == *formatP)
        {
            *colorSpaceP = surfaceFormats[i].colorSpace;
            return;
        }
    }

    printf("surface format %s is not supported\n", getFormatName(*formatP));
    assert(false);
}

void VkDeviceZHU::createSurface()
{
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.flags = 0;
    surfaceInfo.hinstance = GetModuleHandle(NULL);
    surfaceInfo.hwnd = _info.window;

    VkResult result = vkCreateWin32SurfaceKHR(_instance, &surfaceInfo, nullptr, &_surface);
    VK_CHECK(result);
}

bool contains(std::vector<VkExtensionProperties> const& supports,
              std::vector<std::string> const& requires)
{
    for (const auto& name : requires)
    {
        auto it = std::find_if(supports.begin(),
                               supports.end(),
                               [&](const VkExtensionProperties& props) {
            return strcmp(props.extensionName, name.c_str()) == 0;
        });

        if (it == supports.end())
            return false;
    }

    return true;
}

bool contains(VkPhysicalDeviceFeatures const& supports,
              VkPhysicalDeviceFeatures const& requires)
{
    const VkBool32* supportsToggles = reinterpret_cast<const VkBool32*>(&supports);
    const VkBool32* requiresToggles = reinterpret_cast<const VkBool32*>(&requires);

    for (int i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); i++)
    {
        if (!requiresToggles[i])
            continue;

        if (!supportsToggles[i])
            return false;
    }

    return true;
}

struct VulkanMemoryInfo
{
    size_t total = 0;
    size_t dedicated = 0;
    size_t shared = 0;
};

VulkanMemoryInfo getMemoryInfo(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties props = {};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &props);

    VulkanMemoryInfo info;

    for (int i = 0; i < props.memoryTypeCount; i++)
    {
        if (props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        {
            uint32_t heap = props.memoryTypes[i].heapIndex;
            info.dedicated += props.memoryHeaps[heap].size;
        }

        if (props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        {
            uint32_t heap = props.memoryTypes[i].heapIndex;
            info.shared += props.memoryHeaps[heap].size;
        }
    }

    info.total = info.dedicated + info.shared;

    return info;
}

uint32_t getMemoryTypeIndex(VkPhysicalDevice physicalDevice,
                            VkDevice device,
                            VkMemoryPropertyFlags propsFlags,
                            uint32_t typeMask)
{
    VkPhysicalDeviceMemoryProperties props = {};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &props);

    for (int i = 0; i < props.memoryTypeCount; i++)
    {
        VkMemoryType type = props.memoryTypes[i];
        if ((type.propertyFlags & propsFlags) && (i & typeMask))
        {
            return i;
        }
    }

    return UINT32_MAX;
}

const char* sizeFormat(size_t size)
{
    static char buffer[32];

    if (size < 1024)
    {
        sprintf(buffer, "%dB", (int)size);
        return buffer;
    }
    
    if (size < (1024 * 1024.0))
    {
        sprintf(buffer, "%.1fKB", size / 1024.0);
        return buffer;
    }
    
    if (size < (1024 * 1024 * 1024.0))
    {
        sprintf(buffer, "%.1fMB", size / (1024.0 * 1024.0));
        return buffer;
    }
    
    if (size < (1024 * 1024 * 1024 * 1024.0))
    {
        sprintf(buffer, "%.1fGB", size / (1024.0 * 1024.0 * 1024.0));
        return buffer;
    }
    
    sprintf(buffer, "%.1fTB", size / (1024.0 * 1024.0 * 1024.0 * 1024.0));
    return buffer;
}

bool comparePhysicalDevice(VkPhysicalDevice lhs,
                           VkPhysicalDevice rhs)
{
    VkPhysicalDeviceFeatures lhsFeatures = {};
    VkPhysicalDeviceFeatures rhsFeatures = {};

    vkGetPhysicalDeviceFeatures(lhs, &lhsFeatures);
    vkGetPhysicalDeviceFeatures(rhs, &rhsFeatures);

    const VkBool32* lhsToggles = reinterpret_cast<const VkBool32*>(&lhsFeatures);
    const VkBool32* rhsToggles = reinterpret_cast<const VkBool32*>(&rhsFeatures);
    int lhsFeatureCount = 0;
    int rhsFeatureCount = 0;

    for (int i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); i++)
    {
        lhsFeatureCount += (lhsToggles[i] ? 1 : 0);
        rhsFeatureCount += (rhsToggles[i] ? 1 : 0);
    }

    auto lhsMemorySize = getMemoryInfo(lhs).total;
    auto rhsMemorySize = getMemoryInfo(rhs).total;

    return (lhsMemorySize + lhsFeatureCount) > (rhsFeatureCount + rhsMemorySize);
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

    VulkanMemoryInfo memoryInfo = getMemoryInfo(_physicalDevice);

    printf("__________\n");
    printf("  GPU Name: %s\n", props.deviceName);
    printf("    GPU ID: %d\n", props.deviceID);
    printf("GPU Memory: %s\n", sizeFormat(memoryInfo.total));
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

void chooseDeviceExtensions(VkDeviceCreateInfoZHU& info,
                            VkPhysicalDevice physicalDevice,
                            std::vector<const char*>* extensionNamesP)
{
    uint32_t extensionCount = 0;
    VkResult result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    VK_CHECK(result);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());
    VK_CHECK(result);

    std::vector<std::string> extensionNames;

    for (int i = 0; i < info.deviceExtensions.size(); i++)
    {
        if (!findExtensionName(info.deviceExtensions[i].c_str(), extensions))
        {
            printf("Extension %s is not supported\n", info.deviceExtensions[i]);
            continue;
        }

        extensionNames.push_back(info.deviceExtensions[i].c_str());
    }

    for (int i = 0; i < extensions.size(); i++)
    {
        if (findExtensionName("VK_KHR_portability_subset", extensions))
        {
            extensionNames.push_back("VK_KHR_portability_subset");
        }
    }

    info.deviceExtensions = extensionNames;

    for (const auto& extension : info.deviceExtensions)
    {
        extensionNamesP->push_back(extension.c_str());
    }
}

void addDeviceQueueCreateInfos(VkDeviceCreateInfoZHU const& info,
                               uint32_t queueFamilyIndex,
                               std::vector<VkDeviceQueueCreateInfo>* createInfosP)
{
    static float priority = 1.0f;

    VkDeviceQueueCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.queueFamilyIndex = queueFamilyIndex;
    createInfo.queueCount = 1;
    createInfo.pQueuePriorities = &priority;

    createInfosP->push_back(createInfo);
}

void VkDeviceZHU::createLogicalDevice()
{
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

void VkDeviceZHU::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.flags = 0;
    poolInfo.queueFamilyIndex = _queueFamilyIndex;

    VkResult result = vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
    VK_CHECK(result);
}

VkCompositeAlphaFlagBitsKHR getCompositeAlphaFlag(VkPhysicalDevice physicalDevice,
                                                  VkSurfaceKHR surface)
{
    VkSurfaceCapabilitiesKHR caps = {};
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,
                                                                surface,
                                                                &caps);
    VK_CHECK(result);

    std::vector<VkCompositeAlphaFlagBitsKHR> list = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (int i = 0; i < list.size(); i++)
    {
        if (list[i] & caps.supportedCompositeAlpha) 
        {
            return list[i];
        }
    }

    assert(false);

    return VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
}

VkPresentModeKHR getPresentMode(VkPhysicalDevice physicalDevice,
                                 VkSurfaceKHR surface)
{
    uint32_t count = 0;
    VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
    VK_CHECK(result);

    std::vector<VkPresentModeKHR> presentModes;
    presentModes.resize(count);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, presentModes.data());
    VK_CHECK(result);
    assert(count > 0);

    VkPresentModeKHR desiredMode = VK_PRESENT_MODE_MAILBOX_KHR;
    VkPresentModeKHR presentMode = presentModes[0];

    for (int i = 0; i < count; i++)
    {
        if (presentModes[i] == desiredMode)
        {
            presentMode = desiredMode;
            return presentMode;
        }
    }

    presentMode = presentModes[0];

    return presentMode;
}

bool isDepthFormat(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;
    }

    return false;
}

bool isStencilFormat(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
        case VK_FORMAT_S8_UINT:
            return true;
    }

    return false;
}

bool isDepthStencilFormat(VkFormat format)
{
    return isDepthFormat(format) && isStencilFormat(format);
}

long sizeofFormat(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_UINT:
        case VK_FORMAT_R8G8B8A8_SRGB:
        case VK_FORMAT_B8G8R8A8_UNORM:
        case VK_FORMAT_B8G8R8A8_UINT:
        case VK_FORMAT_B8G8R8A8_SRGB:
            return sizeof(char) * 4;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
        case VK_FORMAT_R32G32B32A32_UINT:
            return sizeof(int) * 4;
        case VK_FORMAT_R32_SFLOAT:
        case VK_FORMAT_R32_UINT:
            return sizeof(int);
        default:
            assert(false);
            return 0;
    }
}

void VkDeviceZHU::createDepthImage()
{
    if (_depthFormat == VK_FORMAT_UNDEFINED)
    {
        printf("Depth attachment is disabled\n");
        return;
    }

    assert(false);
}

VkRenderPass createVkRenderPass(VkDevice device,
                                VkFormat colorFormat, 
                                VkFormat depthFormat,
                                VkAttachmentLoadOp loadAction)
{
    bool isDepthEnabled = isDepthFormat(depthFormat);
    uint32_t attachmentCount = isDepthEnabled ? 2 : 1;
    VkAttachmentDescription attachments[2] = { {}, {} };
    VkAttachmentReference references[2] = { {}, {} };

    VkImageLayout initialLayoutColor = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout initialLayoutDepth = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout finalLayoutColor = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    VkImageLayout finalLayoutDepth = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

    if (loadAction == VK_ATTACHMENT_LOAD_OP_LOAD)
    {
        initialLayoutColor = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        initialLayoutDepth = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    }

    if (isDepthStencilFormat(depthFormat))
    {
        finalLayoutDepth = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
    
    VkAttachmentDescription& colorAttachment = attachments[0];
    colorAttachment.flags = 0;
    colorAttachment.format = colorFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = loadAction;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = initialLayoutColor;
    colorAttachment.finalLayout = finalLayoutColor;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    if (isDepthEnabled)
    {
        VkAttachmentDescription& depthStencilAttachment = attachments[1];
        depthStencilAttachment.flags = 0;
        depthStencilAttachment.format = depthFormat;
        depthStencilAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthStencilAttachment.loadOp = loadAction;
        depthStencilAttachment.stencilLoadOp = loadAction;
        depthStencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthStencilAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthStencilAttachment.initialLayout = initialLayoutDepth;
        depthStencilAttachment.finalLayout = finalLayoutDepth;

        VkAttachmentReference& depthStencilReference = references[1];
        depthStencilReference.attachment = 1;
        depthStencilReference.layout = finalLayoutDepth;
    }

    VkSubpassDependency dependencies[2] = { {}, {} };
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = 0;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = 0;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pDepthStencilAttachment = isDepthEnabled ? &references[1] : nullptr;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.pNext = nullptr;
    renderPassInfo.flags = 0;
    renderPassInfo.attachmentCount = attachmentCount;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.dependencyCount = 2;
    renderPassInfo.pDependencies = dependencies;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
    VK_CHECK(result);

    return renderPass;
}

void VkDeviceZHU::createRenderPass()
{
    _renderPassLoad = createVkRenderPass(_device, 
                                         _surfaceFormat,
                                         _depthFormat,
                                         VK_ATTACHMENT_LOAD_OP_LOAD);
    _renderPassClear = createVkRenderPass(_device,
                                          _surfaceFormat,
                                          _depthFormat,
                                          VK_ATTACHMENT_LOAD_OP_CLEAR);
}

VkFramebuffer createVkFramebuffer(VkDevice device,
                                  VkImageView colorImageView,
                                  VkImageView depthImageView,
                                  VkExtent2D extent,
                                  VkRenderPass renderPass,
                                  VkAttachmentLoadOp loadAction)
{
    VkImageView imageViews[2] = {
        colorImageView,
        depthImageView
    };
    uint32_t imageViewCount = depthImageView ? 2 : 1;

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.pNext = nullptr;
    framebufferInfo.flags = 0;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;
    framebufferInfo.attachmentCount = imageViewCount;
    framebufferInfo.pAttachments = imageViews;

    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer);
    VK_CHECK(result);

    return framebuffer;
}

void VkDeviceZHU::createFramebuffers()
{
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

VkImageView createVkImageView(VkDevice device,
                              VkImage image,
                              VkFormat format)
{
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    if (isDepthStencilFormat(format))
    {
        aspectMask = 0;
        if (isDepthFormat(format)) aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        if (isStencilFormat(format)) aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkComponentMapping components;
    components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageSubresourceRange range;
    range.aspectMask = aspectMask;
    range.baseArrayLayer = 0;
    range.baseMipLevel = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;
    range.levelCount = VK_REMAINING_MIP_LEVELS;

    VkImageViewCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange = range;
    info.components = components;

    VkImageView view = VK_NULL_HANDLE;
    VkResult result = vkCreateImageView(device, &info, nullptr, &view);
    VK_CHECK(result);

    return view;
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
    if (_swapchain)
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

        chooseSwapchainProperties();
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.surface = _surface;
    createInfo.minImageCount = 3;
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
    chooseSwapchainProperties();
    createSwapchain();
    createDepthImage();
    createRenderPass();
    createFramebuffers();
}

VkDeviceZHU::~VkDeviceZHU()
{
    for (auto& framebuffer : _framebuffers)
    {
        if (framebuffer)
        {
            vkDestroyFramebuffer(_device, framebuffer, nullptr);
            framebuffer = VK_NULL_HANDLE;
        }
    }

    for (auto& view : _backingImageViews)
    {
        if (view)
        {
            vkDestroyImageView(_device, view, nullptr);
            view = VK_NULL_HANDLE;
        }
    }

    if (_depthImage)
    {
        vkDestroyImageView(_device, _depthImageView, nullptr);
        vkDestroyImage(_device, _depthImage, nullptr);
        vkFreeMemory(_device, _depthImageMemory, nullptr);
        _depthImageView = VK_NULL_HANDLE;
        _depthImage = VK_NULL_HANDLE;
        _depthImageMemory = VK_NULL_HANDLE;
    }

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

    if (_swapchain)
    {
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
        _swapchain = VK_NULL_HANDLE;
    }

    if (_commandPool)
    {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
        _commandPool = VK_NULL_HANDLE;
    }

    if (_device)
    {
        vkDestroyDevice(_device, nullptr);
        _device = VK_NULL_HANDLE;
    }

    if (_surface)
    {
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        _surface = VK_NULL_HANDLE;
    }

    if (_debugUtils)
    {
        _vkDestroyDebugUtilsMessengerEXT(_instance, _debugUtils, nullptr);
        _debugUtils = VK_NULL_HANDLE;
    }

    if (_instance)
    {
        vkDestroyInstance(_instance, nullptr);
        _instance = VK_NULL_HANDLE;
    }
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

void VkDeviceZHU::beginFrame()
{
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
        createRenderPass();
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
        createRenderPass();
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
            createRenderPass();
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

VkFence VkDeviceZHU::allocateFence()
{
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    VkFence fence = VK_NULL_HANDLE;
    VkResult result = vkCreateFence(_device, &info, nullptr, &fence);
    VK_CHECK(result);

    return fence;
}

VkSemaphore VkDeviceZHU::allocateSemaphore()
{
    return 0;
}

void VkDeviceZHU::freeFence(VkFence& fence)
{
    vkDestroyFence(_device, fence, nullptr);
    fence = VK_NULL_HANDLE;
}

void VkDeviceZHU::freeSemaphore(VkSemaphore& semaphore)
{}