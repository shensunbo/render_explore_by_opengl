#pragma once

#include <string>
#include <unordered_map>
#include "gl/gl_headers.h"
#include "CommonDataStruct.h"

class TextureCache {
public:
    TextureCache() = default;
    ~TextureCache();

    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;
    TextureCache(TextureCache&&) = delete;
    TextureCache& operator=(TextureCache&&) = delete;

    // Return existing texture id or create a new one from CPU image data.
    GLuint getOrCreate(const std::string& path, const imageParam& imgData);

    void destroy();

private:
    std::unordered_map<std::string, GLuint> textures_;
};
