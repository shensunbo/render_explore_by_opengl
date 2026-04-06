#pragma once

#include <string>
#include <mutex>
#include <unordered_map>
#include "gl/gl_headers.h"
#include "common/CommonDataStruct.h"
#include <ktx.h>

/**
 * @class TextureCache
 * @brief Manages OpenGL texture objects and caches them by file path.
 *
 * This class provides methods to retrieve or create OpenGL textures from image data,
 * ensuring that each texture is loaded only once and reused as needed.
 */
class TextureCache {
public:
    TextureCache() = default;
    ~TextureCache();

    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;
    TextureCache(TextureCache&&) = delete;
    TextureCache& operator=(TextureCache&&) = delete;

    /**
     * @brief Returns an existing texture ID or creates a new one from CPU image data.
     * @param path File path of the texture.
     * @param imgData Image data parameters.
     * @param srgb Whether to use sRGB color space for the texture.
     * @return OpenGL texture ID.
     */
    GLuint getOrCreate(const std::string& path, const imageParam& imgData, bool srgb = false);

    /**
     * @brief Pre-loads a KTX/KTX2 file from disk and transcodes it (CPU only, no GL).
     *
     * Thread-safe. Can be called from worker threads to parallelise the expensive
     * file I/O and Basis-Universal transcode step. The resulting ktxTexture* is
     * stashed internally and consumed by the next getOrCreateKtx() call for the
     * same path.
     *
     * @param path Full file path of the KTX or KTX2 texture.
     */
    void preloadKtx(const std::string& path);

    /**
     * @brief Returns an existing texture ID or uploads a (possibly pre-loaded) KTX texture.
     *
     * If preloadKtx() was called for this path, the pre-loaded and pre-transcoded
     * data is uploaded directly — only the fast GL upload runs on the calling thread.
     * Otherwise falls back to a full load+transcode+upload.
     * Must be called from the main GL thread.
     *
     * @param path Full file path of the KTX or KTX2 texture.
     * @return OpenGL texture ID, or 0 on failure.
     */
    GLuint getOrCreateKtx(const std::string& path);

    /**
     * @brief Releases all cached textures and clears the cache.
     */
    void destroy();

private:
    std::unordered_map<std::string, GLuint> textures_;

    /// Pre-loaded (file I/O + transcode done) KTX textures awaiting GL upload.
    std::mutex pendingMutex_;
    std::unordered_map<std::string, ktxTexture*> pendingKtx_;
};
