#include "ContextMNRT.h"
#include "RaytracingAPI.h"
#include "BasicToolsRT.h"

ContextMNRT::ContextMNRT(const RaytracingCreateInfo& info,
                         std::vector<RGBA>* pixels,
                         int* width,
                         int* height)
    : _pixels(*pixels)
    , _width(*width)
    , _height(*height)
    , _info(info)
    , _threadPool()
    , _world()
{
    nlohmann::json json;
    std::ifstream input(info.sceneFile);
    input >> json;

    _camera.deserialize(json["camera"]);
    _world.deserialize(json["world"]);

    _width = 1000;
    _height = _width * _camera.aspect();
    _pixels.resize(_width * _height, RGBA{ 0, 0, 0, 255 });
}

ContextMNRT::~ContextMNRT()
{
    _world.release();
    _threadPool.release();
}

RGBA packedRGBA(const glm::vec4& color)
{
    RGBA rgba;
    rgba[0] = color.r * 255.99;
    rgba[1] = color.g * 255.99;
    rgba[2] = color.b * 255.99;
    rgba[3] = color.a * 255.99;

    return rgba;
}

void ContextMNRT::runAsync()
{
    _beginTime = std::chrono::system_clock::now();
    _threadPool.enqueue([this]()->void {
        static const bool vflip = true;
        for (int i = 0; i < _height; i++)
        {
            for (int j = 0; j < _width; j++)
            {
                glm::vec3 color(0.0);

                for (int k = 0; k < NUM_SAMPLES_PER_PIXEL; k++)
                {
                    float jitter_x = NUM_SAMPLES_PER_PIXEL > 1 ? random2() : 0;
                    float jitter_y = NUM_SAMPLES_PER_PIXEL > 1 ? random2() : 0;
                    float x = float(j + jitter_x) / _width;
                    float y = float(i + jitter_y) / _height;
                    x = CLAMP(x, 0.0, 1.0);
                    y = CLAMP(y, 0.0, 1.0);
                    y = vflip ? (1.0 - y) : y;

                    Ray ray = _camera.generateRay(x, y);
                    color += tracePath(ray, 0);
                }

                color /= NUM_SAMPLES_PER_PIXEL;
                color = glm::sqrt(color);

                int index = i * _width + j;
                _pixels[index] = packedRGBA(glm::vec4(color, 1.0));
                _completion += 1;
                _completed.notify_one();
            }
        }
    });
}

void ContextMNRT::waitIdle()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _completed.wait(lock, [&]() { 
        return _completion == (_width * _height); 
    });
    
    auto duration = std::chrono::system_clock::now() - _beginTime;
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    printf("_______________\n");
    printf("MiniRaytracing:\n");
    printf("     Time cost: %dmin %dsec\n", sec / 60, sec % 60);
    printf("\n");
}

glm::vec3 ContextMNRT::tracePath(Ray ray, int depth)
{
    Intersection hit;
    ScatteredResult result;
    result.color = glm::vec3(0.0);

    if (depth > MAX_NUM_DEPTH)
    {
        return result.color;
    }

    if (_world.intersect(ray, 0.0, FLT_MAX, &hit))
    {
        if (hit.material->scatter(ray, hit, &result))
        {
            for (const auto& scatteredRay : result.scatteredRays)
            {
                result.color *= tracePath(scatteredRay, depth + 1);
            }
        }
    }
    else
    {
        result.color = missHit(ray);
    }

    return result.color;
}

glm::vec3 ContextMNRT::missHit(Ray ray)
{
    glm::vec3 dir = glm::normalize(ray.direction);
    float t = (dir.y + 1.0) * 0.5;
    
    return  t * glm::vec3(0.5, 0.7, 1.0) + (1.0f - t) * glm::vec3(1.0);
}