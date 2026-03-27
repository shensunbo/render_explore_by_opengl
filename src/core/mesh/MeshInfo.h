#pragma once
// #include "../mesh.h"
// #include "../shader.h"
#include "shader/Shader.h"
#include "log/mylog.h"
#include "ModelLoader.h"
#include "common/CommonDataStruct.h"
#include "BufferObjectData.h"

/**
 * @class MeshInfo
 * @brief Holds mesh and texture information for a model.
 *
 * This class manages the loading and storage of mesh and texture data for a model,
 * using ModelLoader and BufferObjectData. It provides access to loaded textures and meshes,
 * and exposes utility functions for texture ID management.
 */
class MeshInfo {
public:
    /**
     * @brief Constructs a MeshInfo object and loads the model.
     * @param path Path to the 3D model file.
     * @param vehInfo Reference to configuration parser.
     * @param textureData Map of texture names to image parameters.
     * @param textureCache Reference to the texture cache.
     * @param gamma Enable gamma correction if true.
     */
    MeshInfo(std::string const &path, ConfigParser& vehInfo,
        const std::unordered_map<std::string, imageParam>& textureData,
        TextureCache& textureCache,
        bool gamma = false) : gammaCorrection(gamma)
    {
        loader.LoadModel(path, meshes, vehInfo, textureData, textureCache);
    }

    MeshInfo(const MeshInfo&) = delete;
    MeshInfo& operator=(const MeshInfo&) = delete;
    MeshInfo(MeshInfo&&) = delete;
    MeshInfo& operator=(MeshInfo&&) = delete;

    ~MeshInfo() = default;

    /**
     * @brief Returns the maximum texture ID used by this model.
     * @return Maximum texture ID.
     */
    unsigned int getMaxTextureID() const {
        return maxTextureID;
    }
    // /**
    //  * @brief Draws the model and all its meshes.
    //  * @param shader Pointer to the shader.
    //  */
    // void Draw(Shader *shader)
    // {
    //     for(unsigned int i = 0; i < meshes.size(); i++)
    //         meshes[i].Draw(shader);
    // }

public:
    /**
     * @brief Stores all the textures loaded so far.
     *
     * Optimization to ensure textures are not loaded more than once.
     */
    std::vector<Texture> textures_loaded;

    /**
     * @brief Stores all mesh data for the model.
     */
    std::vector<BufferObjectData> meshes;

    /**
     * @brief Indicates if gamma correction is enabled for this model.
     */
    bool gammaCorrection;

    /**
     * @brief Loader used to load the model data.
     */
    ModelLoader loader;

private:
    /**
     * @brief Maximum texture ID (should be determined automatically in future).
     */
    const unsigned int maxTextureID = 6; ///< TODO: get this automatically
};