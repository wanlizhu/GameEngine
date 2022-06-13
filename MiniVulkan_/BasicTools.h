#pragma once

#include "VulkanAPI.h"

#define SHARED_MEMORY (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
#define PRIVATE_MEMORY (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
#define USAGE_VERTEX_BUFFER   (VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
#define USAGE_INDEX_BUFFER    (VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
#define USAGE_UNIFORM_BUFFER  (VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)

#define ATTRIBUTE_POSITION_NAME "in_position"
#define ATTRIBUTE_TEXCOORD_NAME "in_texcoord"
#define ATTRIBUTE_NORMAL_NAME "in_normal"

#define VS_CONFIG_NAME "config"
#define VS_CONFIG_MODEL_MATRIX_NAME "config.modelMatrix" 
#define VS_CONFIG_VIEW_MATRIX_NAME "config.viewMatrix" 
#define VS_CONFIG_PROJECTION_MATRIX_NAME "config.projectionMatrix" 
#define VS_CONFIG_SUN_POSITION_MATRIX_NAME "config.sunPos" 
#define VS_CONFIG_CAMERA_POSITION_MATRIX_NAME "config.cameraPos" 

#define FS_CONFIG_NAME "config"
#define FS_CONFIG_TECHNIQUE_NAME "config.technique"
#define FS_CONFIG_PHONG_IMAGE_MASK_NAME "config.phong_image_mask"
#define FS_CONFIG_PHONG_AMBIENT_NAME "config.phong_ambient"
#define FS_CONFIG_PHONG_DIFFUSE_NAME "config.phong_diffuse"
#define FS_CONFIG_PHONG_SPECULAR_NAME "config.phong_specular"
#define FS_CONFIG_PHONG_GLOSS_NAME "config.pong_gloss"
#define FS_CONFIG_PBR_IMAGE_MASK_NAME "config.pbr_image_mask"
#define FS_CONFIG_PBR_BASE_COLOR_NAME "config.pbr_basecolor"
#define FS_CONFIG_PBR_METALLIC_NAME "config.pbr_metallic"
#define FS_CONFIG_PBR_ROUGHNESS_NAME "config.pbr_roughness"

#define LIGHT_NAME "light"
#define LIGHT_COUNT_NAME "light.count"
#define LIGHT_POSITIONS_NAME "light.positions"
#define LIGHT_DIRECTIONS_NAME "light.directions"
#define LIGHT_COLORS_NAME "light.colors"
#define LIGHT_ATTENUATIONS "light.attenuations"
#define LIGHT_CUTOFFS "light.cutoffs"

#define strdup _strdup
#define VK_LOAD_FUNC(instance, func) (PFN_##func)vkGetInstanceProcAddr(instance, #func)
#define VK_WHOLE_RECT VkRect2D{ VkOffset2D{0, 0}, VkExtent2D{0, 0} }
#define VK_CHECK(result) if (result != VK_SUCCESS) { \
    assert(false); \
}
#define VK_DEBUG_LOG(format, ...) do { \
    printf(format, __VA_ARGS__); \
} while(false)















struct VkDeviceCreateInfoZHU;

bool loadFile(const std::string& path, std::vector<uint8_t>* bytes);
bool loadTextFile(const std::string& path, std::string* str);
bool startsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);
std::string trim(const std::string& str);
std::string replaceFileExtension(const std::string& path, const std::string& ext);
const char* sizeFormat(size_t size);

bool isDepthFormat(VkFormat format);
bool isStencilFormat(VkFormat format);
bool isDepthStencilFormat(VkFormat format);
long sizeofFormat(VkFormat format);
const char* getFormatName(VkFormat format);

VkAccessFlags getAccessFlagsForImageLayout(VkImageLayout layout);
VkPipelineStageFlags getStageFlagsForImageLayout(VkImageLayout layout);

bool checkVersion(int major, int minor);
bool findLayerName(const char* layerName, 
                   std::vector<VkLayerProperties> const& layers);
bool findExtensionName(const char* extensionName, 
                       std::vector<VkExtensionProperties> const& extensions);
bool findName(const char* name,
              std::vector<std::string> const& list);
void chooseInstanceLayersAndExtensions(VkDeviceCreateInfoZHU& info, 
									   std::vector<const char*>* layerNamesP, 
									   std::vector<const char*>* extensionNamesP);
									   
void findFormatAndColorSpace(VkPhysicalDevice physicalDevice, 
                             VkSurfaceKHR surface, 
							 VkFormat* formatP, 
							 VkColorSpaceKHR* colorSpaceP);
VkCompositeAlphaFlagBitsKHR getCompositeAlphaFlag(VkPhysicalDevice 
                                                  physicalDevice, 
												  VkSurfaceKHR surface);
VkPresentModeKHR getPresentMode(VkPhysicalDevice physicalDevice, 
                                VkSurfaceKHR surface);

bool contains(std::vector<VkExtensionProperties> const& supports, 
              std::vector<std::string> const& requires);
bool contains(VkPhysicalDeviceFeatures const& supports, 
              VkPhysicalDeviceFeatures const& requires);
bool comparePhysicalDevice(VkPhysicalDevice lhs, VkPhysicalDevice rhs);
void chooseDeviceExtensions(VkDeviceCreateInfoZHU& info,
                            VkPhysicalDevice physicalDevice,
                            std::vector<const char*>* extensionNamesP);
void addDeviceQueueCreateInfos(VkDeviceCreateInfoZHU const& info,
                               uint32_t queueFamilyIndex,
                               std::vector<VkDeviceQueueCreateInfo>* createInfosP);

size_t getMemorySizeTotal(VkPhysicalDevice physicalDevice);
uint32_t getMemoryTypeIndex(VkPhysicalDevice physicalDevice,
                            VkDevice device,
                            VkMemoryPropertyFlags propsFlags,
                            uint32_t typeMask);

VkRenderPass createVkRenderPass(VkDevice device,
                                VkFormat colorFormat, 
                                VkFormat depthFormat,
                                VkAttachmentLoadOp loadAction);
VkFramebuffer createVkFramebuffer(VkDevice device,
                                  VkImageView colorImageView,
                                  VkImageView depthImageView,
                                  VkExtent2D extent,
                                  VkRenderPass renderPass,
                                  VkAttachmentLoadOp loadAction);
								  
VkCommandBuffer createAndBeginTransientCommandBuffer(VkDevice device, VkCommandPool commandPool);
void endAndSubmitTransientCommandBuffer(VkDevice device, 
                                        VkQueue queue,
										VkCommandPool commandPool,
										VkCommandBuffer commandBuffer);
								  
VkBuffer createVkBuffer(VkDevice device, size_t size, VkBufferUsageFlags usage);
VkDeviceMemory createAndBindBufferMemory(VkDevice device, 
                                         VkPhysicalDevice physicalDevice,
                                         VkBuffer buffer, 
										 VkMemoryPropertyFlags props);
void updateHostVisibleBuffer(VkDevice device, 
                             VkDeviceMemory memory, 
							 size_t offset, 
							 size_t size, 
							 const void* data);
void updateHostInvisibleBuffer(VkDevice device, 
                               VkPhysicalDevice physicalDevice,
							   VkQueue queue,
                               VkCommandPool commandPool,
                               VkBuffer buffer, 
							   size_t offset, 
							   size_t size, 
							   const void* data); 

VkImage createVkImage(VkDevice device, const VkExtent2D& extent, VkFormat format);
VkImageView createVkImageView(VkDevice device, VkImage image, VkFormat format);
VkDeviceMemory createAndBindImageMemory(VkDevice device, 
                                        VkPhysicalDevice physicalDevice, 
										VkImage image);
void updateImageRect(VkDevice device,
                     VkPhysicalDevice physicalDevice,
					 VkQueue queue,
                     VkCommandPool commandPool,
					 VkImage image,
					 VkFormat format,
                     const VkRect2D& rect,
					 const void* data);
