#pragma once

#include "IImageLoader.h"

class CImageLoader_STB : public IImageLoader
{
public:
    virtual ~CImageLoader_STB() = default;
    virtual Image load(const fs::path& path,
                       bool flipv,
                       IDevice* device) override;
    virtual void save(Image image,
                      const fs::path& path,
                      bool flipv,
                      IDevice* device) override;
};
