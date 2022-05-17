#pragma once

#include "VulkanConfig.h"

struct VkDeviceCreateInfoZHU;

bool loadFile(const std::string& path, std::vector<uint8_t>* bytes);
bool loadTextFile(const std::string& path, std::string* str);
bool beginsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);
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
