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
        mylog(LogLevel::I, "Texture loaded at path: %s, width %d, height %d, channels: %d", filename.c_str(), pngData.width, pngData.height, pngData.nrChannels);
    }
    else
    {
        pngData.data = nullptr;
        mylog(LogLevel::E, "Texture failed to load at path: %s", filename.c_str());
        assert(false);
        return -1;
    }

    return 0;
}