#pragma once

#include "VulkanConfig.h"

enum VkFormatZHU
{
    VK_FORMAT_INT   = VK_FORMAT_R32_SINT,
    VK_FORMAT_UINT  = VK_FORMAT_R32_UINT,
    VK_FORMAT_FLOAT = VK_FORMAT_R32_SFLOAT,

    VK_FORMAT_VEC2  = VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_VEC3  = VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_VEC4  = VK_FORMAT_R32G32B32A32_SFLOAT,

    VK_FORMAT_MAT2  = -2,
    VK_FORMAT_MAT3  = -3,
    VK_FORMAT_MAT4  = -4,
};

struct VkVertexAttributeZHU
{
    std::string name;
    VkFormatZHU type;
    int location;

    bool parse(const std::string& json);
};

struct VkUniformBlockZHU
{
    std::string name;
    int size;
    int set;
    int binding;
    std::unordered_map<std::string, int> offsets;
    std::unordered_map<std::string, VkFormatZHU> types;
    std::unordered_map<std::string, int> arraysizes;

    bool parse(const std::string& json);
};

struct VkReflectionZHU
{
    std::unordered_map<std::string, VkVertexAttributeZHU> vertexAttributes;
    std::unordered_map<std::string, VkUniformBlockZHU> uniformBlocks;

    bool loadFromFile(const std::string& path);
};
