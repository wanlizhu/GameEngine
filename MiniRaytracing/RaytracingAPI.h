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
#include <chrono>
#include <regex>

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
    std::string sceneFile;
};

struct PUBLIC_SYMBOL RaytracingResult
{
    Context* context = nullptr;
    std::vector<RGBA> pixels;
    int width = 0;
    int height = 0;

    virtual ~RaytracingResult();
    void wait();
};

std::shared_ptr<RaytracingResult>
PUBLIC_SYMBOL dispatchRaytracing(const RaytracingCreateInfo& info);

