#pragma once

#include "VulkanConfig.h"

enum VkFormatZHU
{
    VK_FORMAT_MAT2 = -1,
    VK_FORMAT_MAT3 = -2,
    VK_FORMAT_MAT4 = -3,
};

struct VkVertexAttributeZHU
{
    std::string name;
    VkFormat type;
    int location;

    bool parse(const std::string& line);
};

struct VkUniformZHU
{
    std::string name;
    int offset;
    VkFormat type;
    int binding;
    VkShaderStageFlags stages;

    bool parse(const std::string& line);
};

struct VkUniformBlockZHU
{
    std::string name;
    int size;
    int binding;
    VkShaderStageFlags stages;

    bool parse(const std::string& line);
};

struct VkReflectionZHU
{
    std::unordered_map<std::string, VkVertexAttributeZHU> vertexAttributes;
    std::unordered_map<std::string, VkUniformZHU> uniforms;
    std::unordered_map<std::string, VkUniformBlockZHU> uniformBlocks;

    bool loadFromFile(const std::string& path);
};
