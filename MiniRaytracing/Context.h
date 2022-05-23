#pragma once

#include "RaytracingAPI.h"
#include "BasicTools.h"
#include "ThreadPool.h"
#include "WorldBVH.h"
#include "Ray.h"
#include "Camera.h"
#include "OpenGLWindow.h"

class Context
{
public:
    Context(const RaytracingCreateInfo& info,
            std::vector<RGBA>* pixels,
            int* width,
            int* height);
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    virtual ~Context();

    void run_async();
    void wait_idle();

private:
    void save_result();
    void render_tile_radial(glm::ivec2 offset);
    void render_tile(glm::ivec2 offset);
    vec3 trace_path(Ray ray, int depth);
    vec3 miss_hit(Ray ray);

private:
    std::vector<RGBA>& _pixels;
    int& _width;
    int& _height;

    std::shared_ptr<OpenGLWindow> _window;
    std::atomic_int _completion;
    std::string _output_path;

    RaytracingCreateInfo _info;
    ThreadPool _thread_pool;
    WorldBVH _world;
    Camera _camera;
    TIME _begin_time;
};