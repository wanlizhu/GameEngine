#include "VulkanCanvas.h"
#include "VkDeviceZHU.h"
#include "VkCommonZHU.h"

VulkanCanvas::VulkanCanvas(HWND window)
{
    VkDeviceCreateInfoZHU info;
    info.window = window;
    info.versionMajor = 1;
    info.versionMinor = 1;
    info.instanceExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };
    info.deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    info.requiredFeatures = {};

    _device = std::make_shared<VkDeviceZHU>(info);
}

VulkanCanvas::~VulkanCanvas()
{}

void VulkanCanvas::beginPaint(const MatrixMVP& mvp)
{
    _device->beginFrame(mvp);
    _device->beginRenderPass(true);
}

void VulkanCanvas::endPaint()
{
    _device->endRenderPass();
    _device->endFrame();
}

void VulkanCanvas::display()
{
    
}

void VulkanCanvas::resize(int width, int height)
{
    _device->resize(width, height);
}

HashKey computeModelHashKey(const Model& model)
{
    size_t hash = 0xc0ffee;

    hash ^= std::hash<void*>()(model.positions->data());
    hash ^= std::hash<void*>()(model.texcoords->data());
    hash ^= std::hash<void*>()(model.indices->data());
    hash ^= std::hash<size_t>()(model.positions->size());
    hash ^= std::hash<size_t>()(model.texcoords->size());
    hash ^= std::hash<size_t>()(model.indices->size());
    hash ^= std::hash<float>()(model.transform[0][0]);
    hash ^= std::hash<float>()(model.transform[0][1]);
    hash ^= std::hash<float>()(model.transform[0][2]);
    hash ^= std::hash<float>()(model.transform[0][3]);
    hash ^= std::hash<float>()(model.transform[1][0]);
    hash ^= std::hash<float>()(model.transform[1][1]);
    hash ^= std::hash<float>()(model.transform[1][2]);
    hash ^= std::hash<float>()(model.transform[1][3]);
    hash ^= std::hash<float>()(model.transform[2][0]);
    hash ^= std::hash<float>()(model.transform[2][1]);
    hash ^= std::hash<float>()(model.transform[2][2]);
    hash ^= std::hash<float>()(model.transform[2][3]);
    hash ^= std::hash<float>()(model.transform[3][0]);
    hash ^= std::hash<float>()(model.transform[3][1]);
    hash ^= std::hash<float>()(model.transform[3][2]);
    hash ^= std::hash<float>()(model.transform[3][3]);

    for (const auto& mesh : model.meshes) 
    {
        hash ^= std::hash<size_t>()(mesh.primitive);
        hash ^= std::hash<size_t>()(mesh.indexType);
        hash ^= std::hash<size_t>()(mesh.indexOffset);
        hash ^= std::hash<size_t>()(mesh.indexCount);
    }

    return hash;
}

void VulkanCanvas::drawModels(const std::vector<Model>& models, bool cache)
{
    for (int i = 0; i < models.size(); i++)
    {
        HashKey  key = computeModelHashKey(models[i]);
        GPUModel model = {};
        auto iterator = _cachedModelMap.find(key);

        if (iterator == _cachedModelMap.end())
        {
            model = _device->createGPUModel(models[i]);

            if (cache)
            {
                _cachedModelMap[key] = model;
            }
        }
        else
        {
            model = iterator->second;
        }

        _device->drawModel(model);
    }
}

void VulkanCanvas::drawNurbs(const std::vector<Nurbs>& nurbs, bool cache)
{}

void VulkanCanvas::clearCachedObjects()
{
    for (auto& [key, model] : _cachedModelMap)
    {
        _device->deleteGPUModel(model);
    }

    for (auto& [key, nurbs] : _cachedNurbsMap)
    {
        _device->deleteGPUNurbs(nurbs);
    }

    _cachedModelMap.clear();
    _cachedNurbsMap.clear();
}