#pragma once

#include "RaytracingAPI.h"
#include "json.hpp"

#define ENABLE_TILED_RENDERING true
#define USE_FLOAT64 true
#define VERTICAL_FLIP true
#define MAX_NUM_DEPTH 50
#define NUM_SAMPLES_PER_PIXEL 10
#define DEFAULT_CANVAS_WIDTH 1000
#define TILE_WIDTH  32
#define TILE_HEIGHT 32

#define RADIANS(deg) (deg * 3.141592653 / 180.0)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, a, b) (MIN(MAX(x, a), b))
#define SECONDS_SINCE(begin) (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - begin).count())

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

vec2 json_vec2(const nlohmann::json& value);
vec3 json_vec3(const nlohmann::json& value);
vec4 json_vec4(const nlohmann::json& value);