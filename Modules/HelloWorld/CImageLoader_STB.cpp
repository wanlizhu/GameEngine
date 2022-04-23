#include "CImageLoader_STB.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

Image CImageLoader_STB::load(const fs::path& path,
                             bool flipv,
                             IDevice* device)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (file == nullptr)
        return {};
    
    int width = 0;
    int height = 0;
    int comps = 0;
    stbi_uc* data = stbi_load_from_file(file, &width, &height, &comps, 4);
    
    assert(comps == 4);
    
    Image image;
    image.width = width;
    image.height = height;
    image.pixels = std::shared_ptr<Image::RGBA8Unorm>(reinterpret_cast<Image::RGBA8Unorm*>(data),
                                                      [&](Image::RGBA8Unorm* p){ if (p) stbi_image_free(p); });
    
    return image;
}

void CImageLoader_STB::save(Image image,
                            const fs::path& path,
                            bool flipv,
                            IDevice* device)
{
    if (path.extension() == ".png")
    {
        stbi_write_png(path.c_str(), image.width, image.height, 4, image.pixels.get(), 0);
    }
    else if (path.extension() == ".jpg")
    {
        stbi_write_jpg(path.c_str(), image.width, image.height, 4, image.pixels.get(), 0);
    }
    else if (path.extension() == ".bmp")
    {
        stbi_write_bmp(path.c_str(), image.width, image.height, 4, image.pixels.get());
    }
    else if (path.extension() == ".tga")
    {
        stbi_write_tga(path.c_str(), image.width, image.height, 4, image.pixels.get());
    }
    else
    {
        printf("Writing to image with extension %s is not supported.", path.extension().c_str());
    }
}
