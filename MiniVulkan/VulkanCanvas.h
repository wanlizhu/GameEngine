#pragma once

#include "VulkanConfig.h"

class VkDeviceZHU;

class PUBLIC_SYMBOL VulkanCanvas 
{
public:
    VulkanCanvas(HWND window);
    VulkanCanvas(const VulkanCanvas&) = delete;
    VulkanCanvas& operator=(const VulkanCanvas&) = delete;
    virtual ~VulkanCanvas();
    
    void beginPaint(const MatrixMVP& mvp);
    void endPaint();
    void display();
    void resize(int width, int height);

    void drawModels(const std::vector<Model>& models, bool cache);
    void drawNurbs(const std::vector<Nurbs>& nurbs, bool cache);
    void clearCachedObjects();

private:
    ARC<VkDeviceZHU> _device;
    std::map<HashKey, GPUModel> _cachedModelMap;
    std::map<HashKey, GPUNurbs> _cachedNurbsMap;
};