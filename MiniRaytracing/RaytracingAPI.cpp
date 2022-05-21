#include "RaytracingAPI.h"
#include "ContextMNRT.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

void RaytracingResult::save(const std::string& _path)
{
    std::string path = get_available_name(_path);
    assert(!std::filesystem::exists(path));

    if (path.find(".png") == path.size() - 4)
        stbi_write_png(path.c_str(), width, height, 4, pixels.data(), 0);
    else if (path.find(".jpg") == path.size() - 4)
        stbi_write_jpg(path.c_str(), width, height, 4, pixels.data(), 0);
    else if (path.find(".bmp") == path.size() - 4)
        stbi_write_bmp(path.c_str(), width, height, 4, pixels.data());
    else if (path.find(".tga") == path.size() - 4)
        stbi_write_tga(path.c_str(), width, height, 4, pixels.data());
}

std::shared_ptr<RaytracingResult>
dispatchRaytracing(const RaytracingCreateInfo& info)
{
    auto result = std::make_shared<RaytracingResult>();
    result->context = new ContextMNRT(info, 
                                      &result->pixels,
                                      &result->width,
                                      &result->height);
    result->context->run_async();

    return result;
}
