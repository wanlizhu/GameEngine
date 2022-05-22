#pragma once

#include "RaytracingConfig.h"
#include "RaytracingAPI.h"
#include "ThreadPool.h"
#include "WorldBVH.h"
#include "Ray.h"
#include "CameraRT.h"
#include "OpenGLWindow.h"

class ContextMNRT
{
public:
    ContextMNRT(const RaytracingCreateInfo& info,
                std::vector<RGBA>* pixels,
                int* width,
                int* height);
    ContextMNRT(const ContextMNRT&) = delete;
    ContextMNRT& operator=(const ContextMNRT&) = delete;
    virtual ~ContextMNRT();

    void run_async();
    void wait_idle();

private:
    void save_result();
    void render_tile(glm::ivec2 offset, glm::ivec2 extent);
    glm::vec3 trace_path(Ray ray, int depth);
    glm::vec3 miss_hit(Ray ray);

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
    CameraRT _camera;
    TIME _begin_time;
};