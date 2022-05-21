#pragma once

#include "RaytracingConfig.h"
#include "json.hpp"

#define VERTICAL_FLIP true
#define MAX_NUM_DEPTH 50
#define NUM_SAMPLES_PER_PIXEL 10
#define DEFAULT_CANVAS_WIDTH 1000
#define ENABLE_TILED_RENDERING true
#define TILE_WIDTH  32
#define TILE_HEIGHT 32

#define RADIANS(deg) (deg * 3.141592653 / 180.0)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, a, b) (MIN(MAX(x, a), b))

const char* cstr_format(const char*, ...);
std::string get_available_name(const std::string& path);

float random1(); // [ 0, 1]
float random2(); // [-1, 1]
float random_in(float a, float b);
bool is_near_zero(const glm::vec2& vec);
bool is_near_zero(const glm::vec3& vec);
glm::vec2 random_in_unit_circle();
glm::vec2 random_on_unit_circle();
glm::vec3 random_in_unit_sphere();
glm::vec3 random_on_unit_sphere();

glm::vec2 json_vec2(const nlohmann::json& value);
glm::vec3 json_vec3(const nlohmann::json& value);
glm::vec4 json_vec4(const nlohmann::json& value);