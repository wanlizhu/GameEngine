#include "Context.h"
#include "RaytracingAPI.h"
#include "BasicTools.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Context::Context(const RaytracingCreateInfo& info,
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

    _output_path = json["name"].get<std::string>() + ".png";
    _camera.deserialize(json["camera"]);
    _world.deserialize(json["world"]);

    _width = DEFAULT_CANVAS_WIDTH;
    _height = _width * _camera.aspect();
    _pixels.resize(_width * _height, RGBA{ 0, 0, 0, 255 });

    _window = std::make_shared<OpenGLWindow>();
    _window->open(_width, _height);
}

Context::~Context()
{
    _thread_pool.release();
    _world.release();
    _window = nullptr;
}

RGBA packed_rgba(const vec4& color)
{
    RGBA rgba;
    rgba[0] = color.r * 255.99;
    rgba[1] = color.g * 255.99;
    rgba[2] = color.b * 255.99;
    rgba[3] = color.a * 255.99;

    return rgba;
}

void Context::run_async()
{
    printf("__________________\n");
    printf("   MiniRaytracing:\n");
    printf("        Film size: [%d, %d]\n", _width, _height);
    printf("  Max trace depth: %d\n", MAX_NUM_DEPTH);
    printf("Samples per pixel: %d\n", NUM_SAMPLES_PER_PIXEL);
    printf("        Tile size: [%d, %d]\n", ENABLE_TILED_RENDERING ? TILE_WIDTH : 0, ENABLE_TILED_RENDERING ? TILE_HEIGHT : 0);
    printf("          Threads: %d\n", ENABLE_TILED_RENDERING ? _thread_pool.thread_count() : 1);
    printf("  Float precision: %d bits\n", USE_FLOAT64 ? 64 : 32);
    printf("\n");
    printf("Running... ");

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

void Context::render_tile(glm::ivec2 offset, glm::ivec2 extent)
{
    _thread_pool.enqueue([this, offset, extent]()->void {
        int y_bound = MIN(_height, offset.y + extent.y);
        int x_bound = MIN(_width, offset.x + extent.x);
        int completion = 0;

        for (int i = offset.y; i < y_bound; i++)
        {
            for (int j = offset.x; j < x_bound; j++)
            {
                vec3 color(0.0);

                for (int k = 0; k < NUM_SAMPLES_PER_PIXEL; k++)
                {
                    FLOAT jitter_x = NUM_SAMPLES_PER_PIXEL > 1 ? random2() : 0;
                    FLOAT jitter_y = NUM_SAMPLES_PER_PIXEL > 1 ? random2() : 0;
                    FLOAT x = FLOAT(j + jitter_x) / _width;
                    FLOAT y = FLOAT(i + jitter_y) / _height;
                    x = CLAMP(x, 0.0, 1.0);
                    y = CLAMP(y, 0.0, 1.0);
                    y = VERTICAL_FLIP ? (1.0 - y) : y;

                    Ray ray = _camera.generate_ray(x, y);
                    color += trace_path(ray, 0);
                }

                color /= NUM_SAMPLES_PER_PIXEL;
                color = glm::sqrt(color);

                int index = i * _width + j;
                _pixels[index] = packed_rgba(vec4(color, 1.0));
                completion += 1;
            }
        }

        _completion += completion;
    });
}

void Context::wait_idle()
{
    while (_completion < (_width * _height))
    {
        _window->update_event();
        _window->update_title(_begin_time, _completion);
        _window->display(_pixels.data(), _width, _height);
        std::this_thread::yield();
    }
    
    _window->update_title(_begin_time, _completion);

    int sec = SECONDS_SINCE(_begin_time);
    printf("Time cost: %dmin %dsec\n", sec / 60, sec % 60);
    printf("\n");

    save_result();
}

void Context::save_result()
{
    std::string path = get_available_name(_output_path);
    assert(!std::filesystem::exists(path));

    if (path.find(".png") == path.size() - 4)
        stbi_write_png(path.c_str(), _width, _height, 4, _pixels.data(), 0);
    else if (path.find(".jpg") == path.size() - 4)
        stbi_write_jpg(path.c_str(), _width, _height, 4, _pixels.data(), 0);
    else if (path.find(".bmp") == path.size() - 4)
        stbi_write_bmp(path.c_str(), _width, _height, 4, _pixels.data());
    else if (path.find(".tga") == path.size() - 4)
        stbi_write_tga(path.c_str(), _width, _height, 4, _pixels.data());
}

vec3 Context::trace_path(Ray ray, int depth)
{
    Intersection hit;
    ScatteredResult result;
    result.color = vec3(0.0);

    if (depth > MAX_NUM_DEPTH)
    {
        return result.color;
    }

    FLOAT t_min = std::numeric_limits<FLOAT>::epsilon();
    FLOAT t_max = std::numeric_limits<FLOAT>::max();

    if (_world.intersect(ray, t_min, t_max, &hit))
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

vec3 Context::miss_hit(Ray ray)
{
    vec3 dir = glm::normalize(ray.direction);
    FLOAT t = (dir.y + 1.0) * 0.5;
    
    return  t * vec3(0.5, 0.7, 1.0) + FLOAT(1.0 - t) * vec3(1.0);
}