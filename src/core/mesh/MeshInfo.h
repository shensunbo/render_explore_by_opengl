#pragma once
#include "shader/Shader.h"
#include "ModelLoader.h"
#include "common/CommonDataStruct.h"
#include "BufferObjectData.h"
#include <unordered_map>
#include <string>
#include <vector>

/**
 * @class MeshInfo
 * @brief Loads and owns all GPU mesh buffers for a single model.
 *
 * Wraps ModelLoader (PIMPL) so assimp headers don't leak further up the
 * include tree. Destructor is defined in MeshInfo.cpp where ModelLoader's
 * Impl type is reachable via the linker.
 */
class MeshInfo {
public:
    /**
     * @brief Load a model from disk and upload its meshes to the GPU.
     * @param path         Path to the model file.
     * @param cfgInfo      Mesh/material configuration parser.
     * @param textureData  Pre-loaded CPU texture data map.
     * @param textureCache GPU texture cache.
     * @param gamma        Enable gamma correction.
     */
    MeshInfo(const std::string& path,
             ConfigParser& cfgInfo,
             const std::unordered_map<std::string, imageParam>& textureData,
             TextureCache& textureCache,
             bool gamma = false);

    /// Defined in MeshInfo.cpp — ModelLoader dtor must be complete at that point.
    ~MeshInfo();

    MeshInfo(const MeshInfo&)            = delete;
    MeshInfo& operator=(const MeshInfo&) = delete;
    MeshInfo(MeshInfo&&)                 = delete;
    MeshInfo& operator=(MeshInfo&&)      = delete;

    /** @brief All GPU-ready mesh buffers for this model. */
    const std::vector<BufferObjectData>& getMeshes() const { return meshes_; }

    /** @brief Maximum texture slot index used by this model. */
    unsigned int getMaxTextureID() const { return maxTextureID_; }

    /** @brief Whether gamma correction is enabled. */
    bool isGammaCorrected() const { return gammaCorrection_; }

    // Allow Renderer direct mesh access for render loop performance.
    std::vector<BufferObjectData> meshes_;

private:
    ModelLoader       loader_;
    bool              gammaCorrection_{false};
    unsigned int      maxTextureID_{6}; ///< TODO: derive automatically
};