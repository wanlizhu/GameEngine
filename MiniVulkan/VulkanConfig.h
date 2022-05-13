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

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#ifdef MINI_VULKAN_EXPORT
#define PUBLIC_SYMBOL __declspec(dllexport)
#else 
#define PUBLIC_SYMBOL __declspec(dllimport)
#endif

#define VK_LOAD_FUNC(instance, func) (PFN_##func)vkGetInstanceProcAddr(instance, #func)
#define VK_CHECK(result) if (result != VK_SUCCESS) { assert(false); }
#define VK_WHOLE_RECT VkRect2D{ VkOffset2D{0, 0}, VkExtent2D{0, 0} }

template<typename T>
using ARC = std::shared_ptr<T>;

template<typename T>
using OPT = std::optional<T>;

