#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <memory>
#include <algorithm>
#include <optional>
#include <filesystem>

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#ifdef MINI_VULKAN_EXPORT
#define PUBLIC_SYMBOL __declspec(dllexport)
#else 
#define PUBLIC_SYMBOL __declspec(dllimport)
#endif

#define strdup _strdup
#define VK_LOAD_FUNC(instance, func) (PFN_##func)vkGetInstanceProcAddr(instance, #func)
#define VK_WHOLE_RECT VkRect2D{ VkOffset2D{0, 0}, VkExtent2D{0, 0} }
#define VK_CHECK(result) if (result != VK_SUCCESS) { \
    assert(false); \
}
#define VK_DEBUG_LOG(format, ...) do { \
    printf(format, __VA_ARGS__); \
} while(false)

template<typename T>
using ARC = std::shared_ptr<T>;

template<typename T>
using OPT = std::optional<T>;

using ARCData = std::shared_ptr<std::vector<uint8_t>>;
using HashKey = size_t;

enum class Technique
{
    BlinnPhong = 0,
    PBR = 1,
};

struct NonCopyable
{
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

struct MatrixMVP
{
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

struct Nurbs
{
    
};

struct GPUNurbs
{

};

struct Mesh
{
    VkPrimitiveTopology primitive = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkIndexType indexType = VK_INDEX_TYPE_UINT32;
    uint32_t indexOffset = 0;
    uint32_t indexCount = 0;
};

struct Model
{
    ARCData positions;
    ARCData texcoords;
    ARCData normals;
    ARCData indices;
    std::vector<Mesh> meshes;

    glm::mat4 transform = glm::mat4(1.0);
    Technique technique = Technique::BlinnPhong;
};

struct GPUBuffer
{
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct GPUModel
{
    GPUBuffer positions;
    GPUBuffer texcoords;
    GPUBuffer normals;
    GPUBuffer indices;
    std::vector<Mesh> meshes;

    GPUBuffer vsconfig;
    GPUBuffer fsconfig;
    GPUBuffer lights;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};