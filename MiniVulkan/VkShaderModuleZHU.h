#pragma once

#include "VulkanAPI.h"

struct VkShaderModuleCreateInfoZHU
{
    VkDevice device = VK_NULL_HANDLE;
    std::string source_path;
    std::string entry_point;
    std::vector<std::string> search_path;
    std::unordered_map<std::string, std::string> macros;
};

class VkShaderModuleZHU : public std::enable_shared_from_this<VkShaderModuleZHU>
{
public:
    VkShaderModuleZHU(const VkShaderModuleCreateInfoZHU& info);
    VkShaderModuleZHU(const VkShaderModuleZHU&) = delete;
    VkShaderModuleZHU& operator=(const VkShaderModuleZHU&) = delete;
    virtual ~VkShaderModuleZHU();

    uint32_t resource_binding(const std::string& name) const;
    uint32_t resource_size(const std::string& name) const;
    uint32_t member_offset(const std::string& name) const;
    uint32_t member_size(const std::string& name) const;

    VkShaderModule const& shader() const { return _shader; }
    VkShaderStageFlagBits const& stage() const { return _stage; }
    const char* entry_point() const { return _info.entry_point.c_str(); }
	
private:
	void compile();
    void reflect();
	void load_spirv();
    void load_reflection();

private:
    VkShaderModuleCreateInfoZHU _info;
    VkShaderModule _shader = VK_NULL_HANDLE;
    VkShaderStageFlagBits _stage;
	std::vector<uint8_t> _spirv;
    std::unordered_map<std::string, uint32_t> _resource_bindings;
    std::unordered_map<std::string, uint32_t> _resource_sizes;
    std::unordered_map<std::string, uint32_t> _member_offsets;
    std::unordered_map<std::string, uint32_t> _member_sizes;
};

