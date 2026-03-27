#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#include "BufferObjectData.h"
#include "texture/TextureCache.h"
#include <configParser/ConfigParser.h>

/**
 * @class ModelLoader
 * @brief Loads 3D models via Assimp.
 *
 * Uses PIMPL to keep Assimp headers out of consumers.
 * Construct once, call LoadModel, then discard or reuse.
 */
class ModelLoader {
public:
    ModelLoader();
    ~ModelLoader(); ///< Defined in .cpp where Impl is complete.

    ModelLoader(const ModelLoader&)            = delete;
    ModelLoader& operator=(const ModelLoader&) = delete;
    ModelLoader(ModelLoader&&)                 = default;
    ModelLoader& operator=(ModelLoader&&)      = default;

    /**
     * @brief Load a model from disk.
     * @param resPath      Path to the model file.
     * @param meshInfo     Output: list of GPU-ready mesh buffers.
     * @param cfgInfo      Mesh/material configuration parser.
     * @param textureData  Pre-loaded CPU texture data.
     * @param textureCache GPU texture cache.
     * @return true on success.
     */
    bool LoadModel(const std::string& resPath,
                   std::vector<BufferObjectData>& meshInfo,
                   ConfigParser& cfgInfo,
                   const std::unordered_map<std::string, imageParam>& textureData,
                   TextureCache& textureCache);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};