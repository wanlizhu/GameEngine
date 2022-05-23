#include "TextureImage.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureImage::TextureImage(const std::string& path)
{
    if (std::filesystem::exists(path))
    {
        int comps = 0;
        uint8_t* bytes = stbi_load(path.c_str(), &_width, &_height, &comps, 4);
        
        if (bytes)
        {
            _data = std::make_shared<std::vector<RGBA>>();
            _data->resize(_width * _height);

            for (int i = 0; i < _height; i++)
            {
                for (int j = 0; j < _width; j++)
                {
                    uint8_t* src = bytes + (i * _width + j) * comps;
                    RGBA& pixel = (*_data)[i * _width + j];
                    pixel[0] = src[0];
                    pixel[1] = comps >= 2 ? src[1] : 0;
                    pixel[2] = comps >= 3 ? src[2] : 0;
                    pixel[3] = comps >= 4 ? src[3] : 1;
                }
            }

            STBI_FREE(bytes);
        }
    }

    if (_data == nullptr)
    {
        printf("Failed to load texture image: %s\n", path.c_str());
        assert(false);
    }
}

RGBA32 TextureImage::sample(const vec2& uv, const vec3& pos) const
{
    if (_data == nullptr)
        return RGBA32(0, 1, 1, 1); // solid cyan for debugging

    FLOAT u = CLAMP(uv.x, 0.0, 1.0);
    FLOAT v = CLAMP(uv.y, 0.0, 1.0);
    v = VERTICAL_FLIP ? (1.0 - v) : v;

    int i = static_cast<int>(u * _width);
    int j = static_cast<int>(v * _height);

    if (i >= _width)  i = _width - 1;
    if (j >= _height) j = _height - 1;

    static const FLOAT scale = 1.0 / 255.0;
    const RGBA& pixel = *(_data->data() + j * _width + i);

    return RGBA32(pixel[0] * scale, 
                  pixel[1] * scale, 
                  pixel[2] * scale,
                  pixel[3] * scale);
}

std::shared_ptr<Texture> make_image(const std::string& path)
{
    return std::make_shared<TextureImage>(path);
}