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
#define _USE_MATH_DEFINES
#include <math.h>

#include "RaytracingAPI.h"
#include "json.hpp"
#define GLM_FORCE_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

//#define ENABLE_CUDA
#define USE_FLOAT64 true
#define VERTICAL_FLIP true
#define MAX_NUM_DEPTH 50
#define NUM_SAMPLES_PER_PIXEL 50
#define DEFAULT_CANVAS_WIDTH 1000
#define TILE_WIDTH  16
#define TILE_HEIGHT 16

#define RADIANS(deg) (deg * 3.141592653 / 180.0)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, a, b) (MIN(MAX(x, a), b))
#define SECONDS_SINCE(begin) (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - begin).count())

#ifdef ENABLE_CUDA
#include "cuda_runtime.h"
#define CUDA_FUNC __host__ __device__
#define CUDA_CHECK(error) { \
    if (error != cudaSuccess) { \
        fprintf(stderr, "Error: %s:%d ", __FILE__, __LINE__); \
        fprintf(stderr, "code: %d, reason: %s\n", error, cudaGetErrorString(error)); \
    } \
}
#else
#define __host__
#define __device__
#define __global__
#define CUDA_FUNC
#define CUDA_CHECK(result) 
#endif

#if USE_FLOAT64
using FLOAT = double;
using vec2 = glm::dvec2;
using vec3 = glm::dvec3;
using vec4 = glm::dvec4;
using mat2 = glm::dmat2;
using mat3 = glm::dmat3;
using mat4 = glm::dmat4;
#else
using FLOAT = float;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat2 = glm::mat2;
using mat3 = glm::mat3;
using mat4 = glm::mat4;
#endif

using DEPTH_BOUNDS = std::pair<FLOAT, FLOAT>;

class Texture;

const char* cstr_format(const char*, ...);
std::string get_available_name(const std::string& path);

FLOAT random1(); // [ 0, 1]
FLOAT random2(); // [-1, 1]
FLOAT random_in(FLOAT a, FLOAT b);
bool is_near_zero(const vec2& vec);
bool is_near_zero(const vec3& vec);
vec2 random_in_unit_circle();
vec2 random_on_unit_circle();
vec3 random_in_unit_sphere();
vec3 random_on_unit_sphere();
vec4 random_color();

vec2 json_vec2(const nlohmann::json& value);
vec3 json_vec3(const nlohmann::json& value);
vec4 json_vec4(const nlohmann::json& value, FLOAT default_alpha);