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
    // Set srgb=true for color textures (albedo/specular color) so sampling is
    // linearized automatically.
    GLuint getOrCreate(const std::string& path, const imageParam& imgData, bool srgb = false);

    void destroy();

private:
    std::unordered_map<std::string, GLuint> textures_;
};
