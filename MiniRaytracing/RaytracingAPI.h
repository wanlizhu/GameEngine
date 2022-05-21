#pragma once

#include "RaytracingConfig.h"

struct PUBLIC_SYMBOL RaytracingCreateInfo
{
    // basic options
    bool quiet = false;
    VkDeviceZHU* device = nullptr; // optional
    std::string sceneFile;
};

struct PUBLIC_SYMBOL RaytracingResult
{
    ContextMNRT* context = nullptr;
    std::vector<RGBA> pixels;
    int width = 0;
    int height = 0;

    virtual ~RaytracingResult();
    void wait();
    void save(const std::string& path);
};

std::shared_ptr<RaytracingResult>
PUBLIC_SYMBOL dispatchRaytracing(const RaytracingCreateInfo& info);

