#pragma once

#include "VulkanAPI.h"
#include "VkShaderModuleZHU.h"

struct VkPipelineCreateInfoZHU
{
    VkDevice device = VK_NULL_HANDLE;
    std::shared_ptr<VkShaderModuleZHU> vertex_shader;
    std::shared_ptr<VkShaderModuleZHU> fragment_shader;
    VkPipelineVertexInputStateCreateInfo   vertex_input = {};
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    VkPipelineRasterizationStateCreateInfo rasterization = {};
    VkPipelineTessellationStateCreateInfo  tessellation = {};
    VkPipelineMultisampleStateCreateInfo   multisample = {};
    VkPipelineDepthStencilStateCreateInfo  depthstencil = {};
    VkPipelineColorBlendStateCreateInfo color_blend = {};
    VkPipelineViewportStateCreateInfo viewport_scissor = {};
    VkPipelineDynamicStateCreateInfo dynamic = {};
    VkRenderPass render_pass = VK_NULL_HANDLE;
    uint32_t subpass_index = 0;

    std::shared_ptr<std::vector<VkVertexInputBindingDescription>>     _vertex_input_bindings;
    std::shared_ptr<std::vector<VkVertexInputAttributeDescription>>   _vertex_input_attributes;
    std::shared_ptr<std::vector<VkPipelineColorBlendAttachmentState>> _color_blend_attachments;
    std::shared_ptr<std::vector<VkDynamicState>> _states;
    std::shared_ptr<VkViewport> viewport;
    std::shared_ptr<VkRect2D> scissor;
   
    VkPipelineCreateInfoZHU(VkDevice device = VK_NULL_HANDLE);
    void add_shader(const char* path);
    void set_vertex_attribute(uint32_t location, VkFormat format);
    void set_primitive_topology(VkPrimitiveTopology topology);
    void set_raster_state(VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front);
    void set_depth_state(bool enable_test, bool enable_write);
    void set_stencil_state(bool enable_test, bool enable_write);
    void set_color_blend_state(int index, bool enable_blending);
    void set_viewport_state(VkRect2D viewport, std::optional<VkRect2D> scissor);
    void set_render_pass(VkRenderPass render_pass, uint32_t subpass);
};

class VkPipelineZHU : public std::enable_shared_from_this<VkPipelineZHU>
{
public:
    VkPipelineZHU(const VkPipelineCreateInfoZHU& info);
    VkPipelineZHU(const VkPipelineZHU&) = delete;
    VkPipelineZHU& operator=(const VkPipelineZHU&) = delete;
    virtual ~VkPipelineZHU();
    
    VkPipeline const& handle() const { return _pipeline; }
    VkPipelineLayout layout() const { return _layout; }
    VkDescriptorSet  allocate_descriptor_set() const;
    VkShaderModuleZHU* vertex_shader() const { return _info.vertex_shader.get(); }
    VkShaderModuleZHU* fragment_shader() const { return _info.fragment_shader.get(); }

private:
    void define_standard_surface_descriptors();
    void create_pipeline_layout();

private:
    VkPipelineCreateInfoZHU _info;
    VkPipeline _pipeline = VK_NULL_HANDLE;
    VkPipelineLayout _layout = VK_NULL_HANDLE;
    VkDescriptorSetLayout _descriptor_set_layout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayoutBinding> _descriptors;
};

