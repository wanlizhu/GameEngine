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

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#ifdef MINI_RAYTRACING_EXPORT
#define PUBLIC_SYMBOL __declspec(dllexport)
#else 
#define PUBLIC_SYMBOL __declspec(dllimport)
#endif

class VkDeviceZHU;
class ContextMNRT;

struct RaytracingCreateInfo;
struct RaytracingResult;

using RGBA = std::array<uint8_t, 4>;
using TIME = std::chrono::system_clock::time_point;