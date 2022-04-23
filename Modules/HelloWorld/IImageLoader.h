#pragma once

#include "Config.h"
#include "IDevice.h"

struct Image
{
    struct RGBA8Unorm
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };
    
    std::shared_ptr<RGBA8Unorm> pixels;
    int width = 0;
    int height = 0;
};

class IImageLoader
{
public:
    using ARC = std::shared_ptr<IImageLoader>;
    
    virtual ~IImageLoader() = default;
    virtual Image load(const fs::path& path,
                       bool flipv,
                       IDevice* device) = 0;
    virtual void save(Image image,
                      const fs::path& path,
                      bool flipv,
                      IDevice* device) = 0;
};
