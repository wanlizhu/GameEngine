#include "VkCommonZHU.h"
#include "VkDeviceZHU.h"

bool loadFile(const std::string& path, std::vector<uint8_t>* bytes)
{
	FILE* file = fopen(path.c_str(), "rb");

    if (!file)
    {
        printf("Failed to open file: %s\n", path.c_str());
        assert(false);
		return false;
    }
	
	fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    bytes->resize(size);
    long num = fread(bytes->data(), 1, size, file);
    assert(num == size);

    fclose(file);
	
	return true;
}

bool loadTextFile(const std::string& path, std::string* content)
{
	std::vector<uint8_t> bytes;
	
	if (loadFile(path, &bytes))
	{
		return false;
	}
	
	content->assign(bytes.begin(), bytes.end());
	content->push_back('\0');
		
	return true;
}

bool beginsWith(const std::string& str, const std::string& prefix)
{
	return str.find(prefix) == 0;
}

bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.find(suffix) == (str.size() - suffix.size());
}

std::string replaceFileExtension(const std::string& _path, const std::string& ext)
{
    std::filesystem::path path(_path);
    std::string result = _path;

    if (path.has_extension())
    {
        size_t pos = result.find_last_of(".");
        result.erase(pos);
    }

    return result + ext;
}


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
    static std::list<std::string> cache;
    std::vector<const char*> requiredLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    
    uint32_t layerCount = 0;
    VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    VK_CHECK(result);

    std::vector<VkLayerProperties> layers(layerCount);
    result = vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    VK_CHECK(result);

    for (int i = 0; i < requiredLayers.size(); i++)
    {
        if (!findLayerName(requiredLayers[i], layers))
        {
            printf("Layer %s is not supported\n", requiredLayers[i]);
            continue;
        }
    
        layerNamesP->push_back(requiredLayers[i]);
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

size_t getMemorySizeTotal(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties props = {};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &props);

    size_t dedicated = 0;
	size_t shared = 0;

    for (int i = 0; i < props.memoryTypeCount; i++)
    {
        if (props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        {
            uint32_t heap = props.memoryTypes[i].heapIndex;
            dedicated += props.memoryHeaps[heap].size;
        }

        if (props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        {
            uint32_t heap = props.memoryTypes[i].heapIndex;
            shared += props.memoryHeaps[heap].size;
        }
    }

	return dedicated + shared;
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

    auto lhsMemorySize = getMemorySizeTotal(lhs);
    auto rhsMemorySize = getMemorySizeTotal(rhs);

    return (lhsMemorySize + lhsFeatureCount) > (rhsFeatureCount + rhsMemorySize);
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
        case VK_FORMAT_R32_SFLOAT: return sizeof(float);
        case VK_FORMAT_R32G32_SFLOAT: return sizeof(float) * 2;
        case VK_FORMAT_R32G32B32_SFLOAT: return sizeof(float) * 3;
        case VK_FORMAT_R32G32B32A32_SFLOAT: return sizeof(float) * 4;
        case VK_FORMAT_R32_UINT: return sizeof(int);
        case VK_FORMAT_R32G32_UINT: return sizeof(int) * 2;
        case VK_FORMAT_R32G32B32_UINT: return sizeof(int) * 3;
        case VK_FORMAT_R32G32B32A32_UINT: return sizeof(int) * 4;

        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_UINT:
        case VK_FORMAT_R8G8B8A8_SRGB:
        case VK_FORMAT_B8G8R8A8_UNORM:
        case VK_FORMAT_B8G8R8A8_UINT:
        case VK_FORMAT_B8G8R8A8_SRGB:
            return sizeof(char) * 4;

        default:
            assert(false);
            return 0;
    }
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

VkCommandBuffer createAndBeginTransientCommandBuffer(VkDevice device, VkCommandPool commandPool)
{
	VkCommandBufferAllocateInfo cmdbufInfo = {};
    cmdbufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdbufInfo.pNext = nullptr;
    cmdbufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdbufInfo.commandPool = commandPool;
    cmdbufInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkResult result = vkAllocateCommandBuffers(device, &cmdbufInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VK_CHECK(result);
	
	return commandBuffer;
}

void endAndSubmitTransientCommandBuffer(VkDevice device, 
                                        VkQueue queue, 
										VkCommandPool pool, 
										VkCommandBuffer commandBuffer)
{
	VkResult result = vkEndCommandBuffer(commandBuffer);
	VK_CHECK(result);
	
	VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;

    VkFence fence = VK_NULL_HANDLE;
    result = vkCreateFence(device, &fenceInfo, nullptr, &fence);
    VK_CHECK(result);

    result = vkQueueSubmit(queue, 1, &submitInfo, fence);
    VK_CHECK(result);

    result = vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    VK_CHECK(result);

    vkDestroyFence(device, fence, nullptr);
    vkFreeCommandBuffers(device, pool, 1, &commandBuffer);
}

VkBuffer createVkBuffer(VkDevice device, size_t size, VkBufferUsageFlags usage)
{
	VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.flags = 0;
    bufferInfo.size = size;
    bufferInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = nullptr;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
    VK_CHECK(result);
	
	return buffer;
}

VkDeviceMemory createAndBindBufferMemory(VkDevice device, 
                                         VkPhysicalDevice physicalDevice,
                                         VkBuffer buffer, 
										 VkMemoryPropertyFlags props)
{
	VkMemoryRequirements requirements = {};
    vkGetBufferMemoryRequirements(device, buffer, &requirements);

    uint32_t memoryTypeIndex = getMemoryTypeIndex(physicalDevice, 
	                                              device, 
												  props, 
												  requirements.memoryTypeBits);
    assert(memoryTypeIndex != UINT32_MAX);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    allocInfo.allocationSize = requirements.size;

    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult result = vkAllocateMemory(device, &allocInfo, nullptr, &memory);
    VK_CHECK(result);

    result = vkBindBufferMemory(device, buffer, memory, 0);
    VK_CHECK(result);
	
	return memory;
}

void updateHostVisibleBuffer(VkDevice device, 
                             VkDeviceMemory memory, 
							 size_t offset, 
							 size_t size, 
							 const void* data)
{
	void* mapped = nullptr;
    VkResult result = vkMapMemory(device, memory, offset, size, 0, &mapped);
    VK_CHECK(result);

    memcpy(mapped, data, size);
    vkUnmapMemory(device, memory);
}
							 
void updateHostInvisibleBuffer(VkDevice device, 
                               VkPhysicalDevice physicalDevice,
							   VkQueue queue,
                               VkCommandPool commandPool,
                               VkBuffer buffer, 
							   size_t offset, 
							   size_t size, 
							   const void* data)
{
	VkBuffer stagingBuffer = createVkBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	VkDeviceMemory stagingMemory = createAndBindBufferMemory(device, physicalDevice, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	
	updateHostVisibleBuffer(device, stagingMemory, 0, size, data);
	
	VkCommandBuffer commandBuffer = createAndBeginTransientCommandBuffer(device, commandPool);
	
	VkBufferCopy region = {};
    region.srcOffset = 0;
    region.dstOffset = 0;
    region.size = size;
    vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer, 1, &region);
		
	endAndSubmitTransientCommandBuffer(device, queue, commandPool, commandBuffer);
	
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemory, nullptr);
}

VkImage createVkImage(VkDevice device, const VkExtent2D& extent, VkFormat format)
{
	assert(false);
	return 0;
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

VkDeviceMemory createAndBindImageMemory(VkDevice device, 
                                        VkPhysicalDevice physicalDevice,
										VkImage image)
{
	assert(false);
	return 0;
}

void updateImageRect(VkDevice device,
                     VkPhysicalDevice physicalDevice,
					 VkQueue queue,
                     VkCommandPool commandPool,
					 VkImage image,
					 VkFormat format,
                     const VkRect2D& rect,
					 const void* data)
{
	assert(false);
}
