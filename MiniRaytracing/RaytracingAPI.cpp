#include "RaytracingAPI.h"
#include "Context.h"

RaytracingResult::~RaytracingResult()
{
    if (context)
    {
        delete context;
        context = nullptr;
    }
}

void RaytracingResult::wait()
{
    context->wait_idle();
}

std::shared_ptr<RaytracingResult>
dispatchRaytracing(const RaytracingCreateInfo& info)
{
    auto result = std::make_shared<RaytracingResult>();
    result->context = new Context(info,
                                  &result->pixels,
                                  &result->width,
                                  &result->height);
    result->context->run_async();

    return result;
}
