#pragma once

#include <array>
#include <chrono>
#include <memory>

#ifdef MINI_RAYTRACING_EXPORT
#define PUBLIC_SYMBOL __declspec(dllexport)
#else 
#define PUBLIC_SYMBOL __declspec(dllimport)
#endif

class VkDeviceZHU;
class Context;

struct RaytracingCreateInfo;
struct RaytracingResult;

using RGBA = std::array<uint8_t, 4>;
using TIME = std::chrono::system_clock::time_point;

struct PUBLIC_SYMBOL RaytracingCreateInfo
{
    // basic options
    bool quiet = false;
    VkDeviceZHU* device = nullptr; // optional
    const char* sceneFile;
};

struct PUBLIC_SYMBOL RaytracingResult
{
    Context* context = nullptr;
    RGBA* pixels = nullptr;
    int width = 0;
    int height = 0;

    RaytracingResult() = default;
    RaytracingResult(const RaytracingResult&) = delete;
    RaytracingResult& operator=(const RaytracingResult&) = delete;
    virtual ~RaytracingResult();
    void wait();
};

std::shared_ptr<RaytracingResult>
PUBLIC_SYMBOL dispatchRaytracing(const RaytracingCreateInfo& info);

