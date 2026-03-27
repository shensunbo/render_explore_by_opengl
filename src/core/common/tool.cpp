#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "tool.h"
#include "log/mylog.h"

int Tool::ImageFromFile(std::string filename, imageParam& pngData)
{
    unsigned char *data = stbi_load(filename.c_str(), &pngData.width, &pngData.height, &pngData.nrChannels, 0);
    if (data)
    {
        pngData.data = data;
        LOG_I("Texture loaded at path: {}, width {}, height {}, channels: {}", filename, pngData.width, pngData.height, pngData.nrChannels);
    }
    else
    {
        pngData.data = nullptr;
        LOG_E("Texture failed to load at path: {}", filename);
        assert(false);
        return -1;
    }

    return 0;
}