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
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#ifdef MINI_VULKAN_EXPORT
#define PUBLIC_SYMBOL __declspec(dllexport)
#else 
#define PUBLIC_SYMBOL __declspec(dllimport)
#endif

using HashKey = size_t;

class VkDeviceZHU;
class VkBufferZHU;
class VkModelZHU;

enum class Technique
{
    BlinnPhong = 0,
    PBR = 1,
};

struct MatrixMVP
{
    glm::mat4 model_matrix;
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;
};

struct IndexRange
{
    uint32_t offset = 0;
    uint32_t count = 0;
};

struct VkModelDB
{
    std::shared_ptr<std::vector<uint8_t>> positions;
    std::shared_ptr<std::vector<uint8_t>> texcoords;
    std::shared_ptr<std::vector<uint8_t>> normals;
    std::shared_ptr<std::vector<uint8_t>> indices;
    std::unordered_map<VkPrimitiveTopology, IndexRange> primitives;

    glm::mat4 transform = glm::mat4(1.0);
    Technique technique = Technique::BlinnPhong;
};

class PUBLIC_SYMBOL VulkanContext
{
public:
    VulkanContext(HWND window);
    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;
    virtual ~VulkanContext();
    
    void begin_frame(const MatrixMVP& mvp);
    void end_frame();
    void present();
    void resize(int width, int height);

    HashKey draw(const VkModelDB& model);
    void draw(HashKey cache_key);
    void clear_cached_objects();

private:
    std::shared_ptr<VkDeviceZHU> _device;
    std::map<HashKey, std::shared_ptr<VkModelZHU>> _cached_models;
};

std::shared_ptr<VulkanContext> 
PUBLIC_SYMBOL make_vulkan_context(HWND window);