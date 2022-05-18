#pragma once

#include "VulkanConfig.h"
#include "VkReflectionZHU.h"

struct VkShaderModuleCreateInfoZHU
{
    VkDevice device = VK_NULL_HANDLE;
    std::string sourcePath;
	std::string reflectionPath;
    std::string entryPoint;
    std::vector<std::string> searchPath;
    std::unordered_map<std::string, std::string> macros;
};

class VkShaderModuleZHU : public std::enable_shared_from_this<VkShaderModuleZHU>
{
public:
    VkShaderModuleZHU(const VkShaderModuleCreateInfoZHU& info);
    VkShaderModuleZHU(const VkShaderModuleZHU&) = delete;
    VkShaderModuleZHU& operator=(const VkShaderModuleZHU&) = delete;
    virtual ~VkShaderModuleZHU();

    VkShaderModule const& shaderModule() const { return _shaderModule; }
    VkShaderStageFlagBits const& stage() const { return _stage; }
    const char* entryPoint() const { return _info.entryPoint.c_str(); }
	
private:
	void compile();
    void reflect();
	void loadSPIRV();
	void loadReflection(); 

private:
    VkShaderModuleCreateInfoZHU _info;
    VkShaderModule _shaderModule = VK_NULL_HANDLE;
    VkShaderStageFlagBits _stage;
    VkReflectionZHU _reflection;
	std::vector<uint8_t> _spirv;
};

