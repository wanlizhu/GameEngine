#include "pch.h"
#include "BasicTools.h"
#include <random>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

const char* cstr_format(const char* fmt, ...)
{
    static char buffer[1024];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    return buffer;
}

std::string get_available_name(const std::string& path)
{
    auto dir = std::filesystem::path(path).parent_path();
    auto stem = std::filesystem::path(path).stem();
    auto extension = std::filesystem::path(path).extension();
    auto current = dir / (stem.string() + extension.string());
    std::regex regex("(\\([0-9]+\\))$");

    while (std::filesystem::exists(current))
    {
        std::smatch match;
        std::string stemstr = stem.string();

        if (std::regex_search(stemstr, match, regex))
        {
            int num = atoi(match[0].str().c_str() + 1);
            std::string numstr = "(" + std::to_string(num + 1) + ")";
            std::string newstr = std::regex_replace(stemstr, regex, numstr);
            stem = newstr;
        }
        else
        {
            stem += " (1)";
        }

        current = dir / (stem.string() + extension.string());
    }

    return current.string();
}

FLOAT random1()
{
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    static std::default_random_engine gen;

    FLOAT val = dist(gen);
    assert(val >= 0.0 && val <= 1.0);

    return val;
}

FLOAT random2()
{
    static std::uniform_real_distribution<double> dist(-1.0, 1.0);
    static std::default_random_engine gen;

    FLOAT val = dist(gen);
    assert(val >= -1.0 && val <= 1.0);

    return val;
}

FLOAT random_in(FLOAT a, FLOAT b)
{
    FLOAT ratio = random1();
    
    return a + (b - a) * ratio;
}

bool is_near_zero(const vec2& vec)
{
    FLOAT min = MIN(std::abs(vec[0]), std::abs(vec[1]));
    return min < std::numeric_limits<FLOAT>::epsilon();
}

bool is_near_zero(const vec3& vec)
{
    FLOAT min = MIN(MIN(std::abs(vec[0]), std::abs(vec[1])), std::abs(vec[2]));
    return min < std::numeric_limits<FLOAT>::epsilon();
}

vec2 random_in_unit_circle()
{
    vec2 pos;

    do {
        pos = vec2(random2(), random2());
    } while (glm::dot(pos, pos) >= 1.0);

    return pos;
}

vec2 random_on_unit_circle()
{
    return glm::normalize(random_in_unit_circle());
}

vec3 random_in_unit_sphere()
{
    vec3 pos;

    do {
        pos = vec3(random2(), random2(), random2());
    } while (glm::length(pos) >= 1.0);

    return pos;
}

vec3 random_on_unit_sphere()
{
    return glm::normalize(random_in_unit_sphere());
}

vec4 random_color()
{
    return vec4(random1(), random1(), random1(), 1.0);
}

vec2 json_vec2(const nlohmann::json& value)
{
    assert(!value.is_null());
    std::vector<FLOAT> array = value;
    assert(array.size() >= 2);

    vec2 vec;
    vec.x = array.at(0);
    vec.y = array.at(1);

    return vec;
}

vec3 json_vec3(const nlohmann::json& value)
{
    assert(!value.is_null());
    std::vector<FLOAT> array = value;
    assert(array.size() >= 3);

    vec3 vec;
    vec.x = array.at(0);
    vec.y = array.at(1);
    vec.z = array.at(2);

    return vec;
}

vec4 json_vec4(const nlohmann::json& value, FLOAT default_alpha)
{
    assert(!value.is_null());
    std::vector<FLOAT> array = value;
    assert(array.size() >= 3);

    vec4 vec;
    vec.x = array.at(0);
    vec.y = array.at(1);
    vec.z = array.at(2);
    vec.w = array.size() >= 4 ? array.at(3) : default_alpha;

    return vec;
}

