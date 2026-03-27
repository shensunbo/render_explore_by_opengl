#pragma once

#include <string>
#include <unordered_map>
#include "gl/gl_headers.h"
#include "common/CommonDataStruct.h"

/**
 * @class TextureCache
 * @brief Manages OpenGL texture objects and caches them by file path.
 *
 * This class provides methods to retrieve or create OpenGL textures from image data,
 * ensuring that each texture is loaded only once and reused as needed.
 */
class TextureCache {
public:
    /**
     * @brief Default constructor for TextureCache.
     */
    TextureCache() = default;

    /**
     * @brief Destructor for TextureCache. Releases all cached textures.
     */
    ~TextureCache();

    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;
    TextureCache(TextureCache&&) = delete;
    TextureCache& operator=(TextureCache&&) = delete;

    /**
     * @brief Returns an existing texture ID or creates a new one from CPU image data.
     *
     * If the texture for the given path already exists, returns its OpenGL ID.
     * Otherwise, creates a new texture from the provided image data.
     * Set srgb=true for color textures (albedo/specular color) so sampling is linearized automatically.
     *
     * @param path File path of the texture.
     * @param imgData Image data parameters.
     * @param srgb Whether to use sRGB color space for the texture.
     * @return OpenGL texture ID.
     */
    GLuint getOrCreate(const std::string& path, const imageParam& imgData, bool srgb = false);

    /**
     * @brief Releases all cached textures and clears the cache.
     */
    void destroy();

private:
    /**
     * @brief Map from texture file path to OpenGL texture ID.
     */
    std::unordered_map<std::string, GLuint> textures_;
};
