#include "VkPipelineZHU.h"
#include "VkCommonZHU.h"

VkPipelineCreateInfoZHU::VkPipelineCreateInfoZHU(VkDevice _device)
    : device(_device)
    , vertexShaderModule(nullptr)
    , fragmentShaderModule(nullptr)
{
    vertexInput = {};
    inputAssembly = {};
    rasterization = {};
    depthstencil = {};
    dynamic = {};
    colorBlend = {};

    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.pNext = nullptr;
    vertexInput.flags = 0;
    vertexInput.vertexAttributeDescriptionCount = 0;
    vertexInput.pVertexAttributeDescriptions = nullptr;
    vertexInput.vertexBindingDescriptionCount = 0;
    vertexInput.pVertexBindingDescriptions = nullptr;
    
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext = nullptr;
    inputAssembly.flags = 0;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.pNext = nullptr;
    rasterization.flags = 0;
    rasterization.depthClampEnable = VK_FALSE;
    rasterization.rasterizerDiscardEnable = VK_FALSE;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_NONE;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.depthBiasEnable = VK_FALSE;
    rasterization.depthBiasConstantFactor = 0.0;
    rasterization.depthBiasClamp = 0.0;
    rasterization.depthBiasSlopeFactor = 0.0;
    rasterization.lineWidth = 1.0;

    tessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellation.pNext = nullptr;
    tessellation.flags = 0;
    tessellation.patchControlPoints = 0;

    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.pNext = nullptr;
    multisample.flags = 0;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample.sampleShadingEnable = VK_FALSE;
    multisample.minSampleShading = 0.0;
    multisample.pSampleMask = nullptr;
    multisample.alphaToCoverageEnable = VK_FALSE;
    multisample.alphaToOneEnable = VK_FALSE;

    depthstencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthstencil.pNext = nullptr;
    depthstencil.flags = 0;
    depthstencil.depthTestEnable = VK_FALSE;
    depthstencil.depthWriteEnable = VK_FALSE;
    depthstencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
    depthstencil.depthBoundsTestEnable = VK_FALSE;
    depthstencil.stencilTestEnable = VK_FALSE;
    depthstencil.front = {};
    depthstencil.back = {};
    depthstencil.minDepthBounds = 0.0;
    depthstencil.maxDepthBounds = 1.0;

    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.pNext = nullptr;
    colorBlend.flags = 0;
    colorBlend.logicOpEnable = VK_FALSE;
    colorBlend.logicOp = VK_LOGIC_OP_AND;
    colorBlend.attachmentCount = 0;
    colorBlend.pAttachments = nullptr;
    colorBlend.blendConstants[0] = 0.0;
    colorBlend.blendConstants[1] = 0.0;
    colorBlend.blendConstants[2] = 0.0;
    colorBlend.blendConstants[3] = 0.0;

    viewportscissor.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportscissor.pNext = nullptr;
    viewportscissor.flags = 0;
    viewportscissor.viewportCount = 0;
    viewportscissor.pViewports = nullptr;
    viewportscissor.scissorCount = 0;
    viewportscissor.pScissors = nullptr;

    _vertexInputAttributes = std::make_shared<std::vector<VkVertexInputAttributeDescription>>();
    _vertexInputBindings = std::make_shared<std::vector<VkVertexInputBindingDescription>>();
    _colorBlendAttachments = std::make_shared<std::vector<VkPipelineColorBlendAttachmentState>>();
    _states = std::make_shared<std::vector<VkDynamicState>>();

    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.pNext = nullptr;
    dynamic.flags = 0;
    dynamic.dynamicStateCount = _states->size();
    dynamic.pDynamicStates = _states->data();
}

void VkPipelineCreateInfoZHU::addShaderModule(const char* path)
{
    VkShaderModuleCreateInfoZHU shaderInfo = {};
    shaderInfo.device = device;
    shaderInfo.sourcePath = path;
    shaderInfo.entryPoint = "main";
    shaderInfo.macros = {};
    shaderInfo.searchPath = {};

    auto shaderModule = std::make_shared<VkShaderModuleZHU>(shaderInfo);

    switch (shaderModule->stage())
    {
        case VK_SHADER_STAGE_VERTEX_BIT:
            vertexShaderModule = shaderModule;
            break;

        case VK_SHADER_STAGE_FRAGMENT_BIT:
            fragmentShaderModule = shaderModule;
            break;

        default:
            assert(false);
            break;
    }
}

void VkPipelineCreateInfoZHU::setVertexAttribute(uint32_t location, VkFormat format)
{
    VkVertexInputAttributeDescription attribute = {};
    attribute.location = location;
    attribute.offset = 0;
    attribute.format = format;
    attribute.binding = location;

    VkVertexInputBindingDescription binding = {};
    binding.binding = location;
    binding.stride = sizeofFormat(format);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    _vertexInputAttributes->push_back(attribute);
    _vertexInputBindings->push_back(binding);
    
    vertexInput.vertexAttributeDescriptionCount = _vertexInputAttributes->size();
    vertexInput.pVertexAttributeDescriptions = _vertexInputAttributes->data();
    vertexInput.vertexBindingDescriptionCount = _vertexInputBindings->size();
    vertexInput.pVertexBindingDescriptions = _vertexInputBindings->data();
}

void VkPipelineCreateInfoZHU::setPrimitiveTopology(VkPrimitiveTopology topology)
{
    inputAssembly.topology = topology;
}

void VkPipelineCreateInfoZHU::setRasterState(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace front)
{
    rasterization.polygonMode = polygonMode;
    rasterization.cullMode = cullMode;
    rasterization.frontFace = front;
}

void VkPipelineCreateInfoZHU::setDepthState(bool enableTest, bool enableWrite)
{
    depthstencil.depthTestEnable = enableTest ? VK_TRUE : VK_FALSE;
    depthstencil.depthWriteEnable = enableWrite ? VK_TRUE : VK_FALSE;
    
    if (enableTest)
    {
        depthstencil.depthCompareOp = VK_COMPARE_OP_LESS;
    }
}

void VkPipelineCreateInfoZHU::setStencilState(bool enableTest, bool enableWrite)
{
    depthstencil.stencilTestEnable = enableTest ? VK_TRUE : VK_FALSE;
   
    if (enableTest)
    {
        depthstencil.front.failOp = VK_STENCIL_OP_KEEP;
        depthstencil.front.passOp = VK_STENCIL_OP_KEEP;
        depthstencil.front.depthFailOp = VK_STENCIL_OP_KEEP;
        depthstencil.front.compareOp = VK_COMPARE_OP_EQUAL;
        depthstencil.front.compareMask = UINT32_MAX;
        depthstencil.front.writeMask = enableWrite ? UINT32_MAX : 0;
        depthstencil.front.reference = 0;
        depthstencil.back = depthstencil.front;
    }

    if (enableWrite)
    {
        depthstencil.front.failOp = VK_STENCIL_OP_KEEP;
        depthstencil.front.passOp = VK_STENCIL_OP_REPLACE;
        depthstencil.front.depthFailOp = VK_STENCIL_OP_KEEP;
        depthstencil.front.compareOp = VK_COMPARE_OP_ALWAYS;
        depthstencil.front.compareMask = UINT32_MAX;
        depthstencil.front.writeMask = enableWrite ? UINT32_MAX : 0;
        depthstencil.front.reference = 0;
        depthstencil.back = depthstencil.front;
    }
}

void VkPipelineCreateInfoZHU::setColorBlendState(int index, bool enableBlending)
{
    VkPipelineColorBlendAttachmentState attachment = {};
    attachment.blendEnable = enableBlending ? VK_TRUE : VK_FALSE;

    if (enableBlending)
    {
        attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        attachment.colorBlendOp = VK_BLEND_OP_ADD;
        attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachment.alphaBlendOp = VK_BLEND_OP_ADD;
        attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    }

    _colorBlendAttachments->push_back(attachment);

    colorBlend.attachmentCount = _colorBlendAttachments->size();
    colorBlend.pAttachments = _colorBlendAttachments->data();
}

void VkPipelineCreateInfoZHU::setViewportState(VkRect2D _viewport, OPT<VkRect2D> _scissor)
{
    viewport = std::make_shared<VkViewport>();
    viewport->x = _viewport.offset.x;
    viewport->y = _viewport.offset.y;;
    viewport->width = _viewport.extent.width;
    viewport->height = _viewport.extent.height;
    viewport->minDepth = 0.0;
    viewport->maxDepth = 1.0;

    if (_scissor.has_value())
    {
        scissor = std::make_shared<VkRect2D>(_scissor.value());
    }
    
    if (viewport)
    {
        viewportscissor.viewportCount = 1;
        viewportscissor.pViewports = viewport.get();
    }

    if (scissor)
    {
        viewportscissor.scissorCount = 1;
        viewportscissor.pScissors = scissor.get();
    }
}

void VkPipelineCreateInfoZHU::setRenderPass(VkRenderPass _renderPass, uint32_t _subpass)
{
    renderPass = _renderPass;
    subpassIndex = _subpass;
}

VkPipelineZHU::VkPipelineZHU(const VkPipelineCreateInfoZHU& info)
    : _info(info)
{
    defineStandardSurfaceDescriptors();
    createPipelineLayout();

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos(2);
    shaderStageInfos[0] = {};
    shaderStageInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageInfos[0].module = info.vertexShaderModule->shaderModule();
    shaderStageInfos[0].pName = info.vertexShaderModule->entryPoint();
    shaderStageInfos[1] = {};
    shaderStageInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageInfos[1].module = info.fragmentShaderModule->shaderModule();
    shaderStageInfos[1].pName = info.fragmentShaderModule->entryPoint();

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.stageCount = shaderStageInfos.size();
    createInfo.pStages = shaderStageInfos.data();
    createInfo.pVertexInputState = &_info.vertexInput;
    createInfo.pInputAssemblyState = &_info.inputAssembly;
    createInfo.pTessellationState = &_info.tessellation;
    createInfo.pViewportState = &_info.viewportscissor;
    createInfo.pRasterizationState = &_info.rasterization;
    createInfo.pMultisampleState = &_info.multisample;
    createInfo.pDepthStencilState = &_info.depthstencil;
    createInfo.pColorBlendState = &_info.colorBlend;
    createInfo.pDynamicState = &_info.dynamic;
    createInfo.layout = _layout;
    createInfo.renderPass = _info.renderPass;
    createInfo.subpass = _info.subpassIndex;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = 0;

    VkResult result = vkCreateGraphicsPipelines(_info.device, nullptr, 1, &createInfo, nullptr, &_pipeline);
    VK_CHECK(result);
}

VkPipelineZHU::~VkPipelineZHU()
{
    if (_setLayout)
    {
        vkDestroyDescriptorSetLayout(_info.device, _setLayout, nullptr);
        _setLayout = VK_NULL_HANDLE;
    }

    if (_layout)
    {
        vkDestroyPipelineLayout(_info.device, _layout, nullptr);
        _layout = VK_NULL_HANDLE;
    }

    if (_pipeline)
    {
        vkDestroyPipeline(_info.device, _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
    }
}

VkDescriptorSetLayoutBinding createDescriptor(const char* stageNames,
                                              uint32_t bindingIndex,
                                              VkDescriptorType type)
{
    VkShaderStageFlags stageFlags = 0;

    if (strchr(stageNames, 'V') || strchr(stageNames, 'v')) 
        stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;

    if (strchr(stageNames, 'F') || strchr(stageNames, 'f'))
        stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

    assert(stageFlags != 0);

    VkDescriptorSetLayoutBinding desc;
    desc.stageFlags = stageFlags;
    desc.binding = bindingIndex;
    desc.descriptorType = type;
    desc.descriptorCount = 1;
    desc.pImmutableSamplers = nullptr;

    return desc;
}

void VkPipelineZHU::defineStandardSurfaceDescriptors()
{
    _descriptors.clear();
    _descriptors.push_back(createDescriptor("V", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    _descriptors.push_back(createDescriptor("F", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    _descriptors.push_back(createDescriptor("F", 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
}

void VkPipelineZHU::createPipelineLayout()
{
    assert(!_descriptors.empty());

    VkDescriptorSetLayoutCreateInfo setLayoutInfo = {};
    setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setLayoutInfo.pNext = nullptr;
    setLayoutInfo.flags = 0;
    setLayoutInfo.bindingCount = _descriptors.size();
    setLayoutInfo.pBindings = _descriptors.data();

    VkResult result = vkCreateDescriptorSetLayout(_info.device, &setLayoutInfo, nullptr, &_setLayout);
    VK_CHECK(result);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_setLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    result = vkCreatePipelineLayout(_info.device, &pipelineLayoutInfo, nullptr, &_layout);
    VK_CHECK(result);
}
