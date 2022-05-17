#pragma once

#include "VulkanConfig.h"
#include "VkShaderModuleZHU.h"

struct VkPipelineCreateInfoZHU
{
    VkDevice device = VK_NULL_HANDLE;
    ARC<VkShaderModuleZHU> vertexShaderModule;
    ARC<VkShaderModuleZHU> fragmentShaderModule;
    VkPipelineVertexInputStateCreateInfo   vertexInput = {};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    VkPipelineRasterizationStateCreateInfo rasterization = {};
    VkPipelineTessellationStateCreateInfo  tessellation = {};
    VkPipelineMultisampleStateCreateInfo   multisample = {};
    VkPipelineDepthStencilStateCreateInfo  depthstencil = {};
    VkPipelineColorBlendStateCreateInfo colorBlend = {};
    VkPipelineViewportStateCreateInfo viewportscissor = {};
    VkPipelineDynamicStateCreateInfo dynamic = {};
    VkRenderPass renderPass = VK_NULL_HANDLE;
    uint32_t subpassIndex = 0;

    std::shared_ptr<std::vector<VkVertexInputBindingDescription>>     _vertexInputBindings;
    std::shared_ptr<std::vector<VkVertexInputAttributeDescription>>   _vertexInputAttributes;
    std::shared_ptr<std::vector<VkPipelineColorBlendAttachmentState>> _colorBlendAttachments;
    std::shared_ptr<std::vector<VkDynamicState>> _states;
    std::shared_ptr<VkViewport> viewport;
    std::shared_ptr<VkRect2D> scissor;
   
    VkPipelineCreateInfoZHU(VkDevice device = VK_NULL_HANDLE);
    void addShaderModule(const char* path);
    void setVertexAttribute(uint32_t location, VkFormat format);
    void setPrimitiveTopology(VkPrimitiveTopology topology);
    void setRasterState(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace front);
    void setDepthState(bool enableTest, bool enableWrite);
    void setStencilState(bool enableTest, bool enableWrite);
    void setColorBlendState(int index, bool enableBlending);
    void setViewportState(VkRect2D viewport, OPT<VkRect2D> scissor = std::nullopt);
    void setRenderPass(VkRenderPass renderPass, uint32_t subpass);
};

class VkPipelineZHU : public std::enable_shared_from_this<VkPipelineZHU>
{
public:
    VkPipelineZHU(const VkPipelineCreateInfoZHU& info);
    VkPipelineZHU(const VkPipelineZHU&) = delete;
    VkPipelineZHU& operator=(const VkPipelineZHU&) = delete;
    virtual ~VkPipelineZHU();
    
    VkPipeline const& handle() const { return _pipeline; }
    VkPipelineLayout const& layout() const { return _layout; }
    VkDescriptorSetLayout const& setLayout() const { return _setLayout; }
    VkShaderModuleZHU* vertexShaderModule() const { return _info.vertexShaderModule.get(); }
    VkShaderModuleZHU* fragmentShaderModule() const { return _info.fragmentShaderModule.get(); }

private:
    void defineStandardSurfaceDescriptors();
    void createPipelineLayout();

private:
    VkPipelineCreateInfoZHU _info;
    VkPipeline _pipeline = VK_NULL_HANDLE;
    VkPipelineLayout _layout = VK_NULL_HANDLE;
    VkDescriptorSetLayout _setLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayoutBinding> _descriptors;
};

