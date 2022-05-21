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
    , _thread_pool()
    , _world()
{
    nlohmann::json json;
    std::ifstream input(info.sceneFile);
    input >> json;

    _camera.deserialize(json["camera"]);
    _world.deserialize(json["world"]);

    _width = DEFAULT_CANVAS_WIDTH;
    _height = _width * _camera.aspect();
    _pixels.resize(_width * _height, RGBA{ 0, 0, 0, 255 });

    _window = std::make_shared<OpenGLWindow>();
    _window->open(_width, _height);
}

ContextMNRT::~ContextMNRT()
{
    _thread_pool.release();
    _world.release();
    _window = nullptr;
}

RGBA packed_RGBA(const glm::vec4& color)
{
    RGBA rgba;
    rgba[0] = color.r * 255.99;
    rgba[1] = color.g * 255.99;
    rgba[2] = color.b * 255.99;
    rgba[3] = color.a * 255.99;

    return rgba;
}

void ContextMNRT::run_async()
{
    _begin_time = std::chrono::system_clock::now();
    const glm::ivec2 tile(TILE_WIDTH, TILE_HEIGHT);
   
    int tiled_height = ((_height + TILE_HEIGHT - 1) / TILE_HEIGHT) * TILE_HEIGHT;
    int tiled_width = ((_width + TILE_WIDTH - 1) / TILE_WIDTH) * TILE_WIDTH;

    if (ENABLE_TILED_RENDERING)
    {
        for (int i = 0; i < tiled_height; i += TILE_HEIGHT)
        {
            for (int j = 0; j < tiled_width; j += TILE_WIDTH)
            {
                render_tile(glm::ivec2(j, i), tile);
            }
        }
    }
    else
    {
        render_tile(glm::ivec2(0, 0), glm::ivec2(_width, _height));
    }
}

void ContextMNRT::render_tile(glm::ivec2 offset, glm::ivec2 extent)
{
    _thread_pool.enqueue([this, offset, extent]()->void {
        int y_bound = MIN(_height, offset.y + extent.y);
        int x_bound = MIN(_width, offset.x + extent.x);
        int completion = 0;

        for (int i = offset.y; i < y_bound; i++)
        {
            for (int j = offset.x; j < x_bound; j++)
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
                    y = VERTICAL_FLIP ? (1.0 - y) : y;

                    Ray ray = _camera.generate_ray(x, y);
                    color += trace_path(ray, 0);
                }

                color /= NUM_SAMPLES_PER_PIXEL;
                color = glm::sqrt(color);

                int index = i * _width + j;
                _pixels[index] = packed_RGBA(glm::vec4(color, 1.0));
                completion += 1;
            }
        }

        _completion += completion;
    });
}

void ContextMNRT::wait_idle()
{
    while (_completion < (_width * _height))
    {
        _window->update_event();
        _window->update_title(_completion);
        _window->display(_pixels.data(), _width, _height);
        std::this_thread::yield();
    }
    
    auto duration = std::chrono::system_clock::now() - _begin_time;
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    printf("_______________\n");
    printf("MiniRaytracing:\n");
    printf("     Time cost: %dmin %dsec\n", sec / 60, sec % 60);
    printf("\n");
}

glm::vec3 ContextMNRT::trace_path(Ray ray, int depth)
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
                result.color *= trace_path(scatteredRay, depth + 1);
            }
        }
    }
    else
    {
        result.color = miss_hit(ray);
    }

    return result.color;
}

glm::vec3 ContextMNRT::miss_hit(Ray ray)
{
    glm::vec3 dir = glm::normalize(ray.direction);
    float t = (dir.y + 1.0) * 0.5;
    
    return  t * glm::vec3(0.5, 0.7, 1.0) + (1.0f - t) * glm::vec3(1.0);
}