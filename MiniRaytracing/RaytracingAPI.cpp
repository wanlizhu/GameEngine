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
    context->waitIdle();
}

std::string getAvailableName(const std::string& path)
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

void RaytracingResult::save(const std::string& _path)
{
    std::string path = getAvailableName(_path);
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
    result->context->runAsync();

    return result;
}
