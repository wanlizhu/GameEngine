#pragma once

#include "RaytracingConfig.h"
#include "RaytracingAPI.h"
#include "ThreadPool.h"
#include "WorldBVH.h"
#include "Ray.h"
#include "CameraRT.h"

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

    void runAsync();
    void waitIdle();

private:
    glm::vec3 tracePath(Ray ray, int depth);
    glm::vec3 missHit(Ray ray);

private:
    std::vector<RGBA>& _pixels;
    int& _width;
    int& _height;

    std::atomic_int _completion;
    std::condition_variable _completed;
    std::mutex _mutex;

    RaytracingCreateInfo _info;
    ThreadPool _threadPool;
    WorldBVH _world;
    CameraRT _camera;
    TIME _beginTime;
};