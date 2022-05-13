#pragma once

#include "VulkanConfig.h"
#include "VkDeviceZHU.h"

class PUBLIC_SYMBOL VulkanCanvas 
{
public:
    VulkanCanvas(HWND window);
    VulkanCanvas(const VulkanCanvas&) = delete;
    VulkanCanvas& operator=(const VulkanCanvas&) = delete;
    virtual ~VulkanCanvas();
    
    void beginPaint();
    void endPaint();
    void display();
    void resize(int width, int height);

private:
    ARC<VkDeviceZHU> _device;
    
};