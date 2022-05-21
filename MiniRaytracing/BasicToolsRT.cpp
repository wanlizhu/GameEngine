#include "BasicToolsRT.h"
#include <random>

float random1()
{
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    static std::default_random_engine gen;

    float val = dist(gen);
    assert(val >= 0.0 && val <= 1.0);

    return val;
}

float random2()
{
    static std::uniform_real_distribution<double> dist(-1.0, 1.0);
    static std::default_random_engine gen;

    float val = dist(gen);
    assert(val >= -1.0 && val <= 1.0);

    return val;
}

float random_in(float a, float b)
{
    float ratio = random1();
    
    return a + (b - a) * ratio;
}

bool is_near_zero(const glm::vec2& vec)
{
    float min = MIN(std::abs(vec[0]), std::abs(vec[1]));
    return min < std::numeric_limits<float>::epsilon();
}

bool is_near_zero(const glm::vec3& vec)
{
    float min = MIN(MIN(std::abs(vec[0]), std::abs(vec[1])), std::abs(vec[2]));
    return min < std::numeric_limits<float>::epsilon();
}

glm::vec2 random_in_unit_circle()
{
    glm::vec2 pos;

    do {
        pos = glm::vec2(random2(), random2());
    } while (glm::dot(pos, pos) >= 1.0);

    return pos;
}

glm::vec2 random_on_unit_circle()
{
    return glm::normalize(random_in_unit_circle());
}

glm::vec3 random_in_unit_sphere()
{
    glm::vec3 pos;

    do {
        pos = glm::vec3(random2(), random2(), random2());
    } while (glm::length(pos) >= 1.0);

    return pos;
}

glm::vec3 random_on_unit_sphere()
{
    return glm::normalize(random_in_unit_sphere());
}

glm::vec2 json_vec2(const nlohmann::json& value)
{
    assert(!value.is_null());

    glm::vec2 vec;
    vec.x = value.at(0);
    vec.y = value.at(1);

    return vec;
}

glm::vec3 json_vec3(const nlohmann::json& value)
{
    assert(!value.is_null());

    glm::vec3 vec;
    vec.x = value.at(0);
    vec.y = value.at(1);
    vec.z = value.at(2);

    return vec;
}

glm::vec4 json_vec4(const nlohmann::json& value)
{
    assert(!value.is_null());

    glm::vec4 vec;
    vec.x = value.at(0);
    vec.y = value.at(1);
    vec.z = value.at(2);
    vec.w = value.at(3);

    return vec;
}