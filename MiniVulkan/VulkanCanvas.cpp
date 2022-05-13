#include "VulkanCanvas.h"

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

void VulkanCanvas::beginPaint()
{
    _device->beginFrame();
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