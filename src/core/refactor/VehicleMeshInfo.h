#pragma once
// #include "../mesh.h"
// #include "../shader.h"
#include "VehicleShader.h"
#include "log/mylog.h"
#include "ModelLoader.h"
#include "CommonDataStruct.h"
#include "BufferObjectData.h"

/**
 * @class VehicleMeshInfo
 * @brief Holds mesh and texture information for a vehicle model.
 *
 * This class manages the loading and storage of mesh and texture data for a vehicle,
 * using ModelLoader and BufferObjectData. It provides access to loaded textures and meshes,
 * and exposes utility functions for texture ID management.
 */
class VehicleMeshInfo {
public:
    /**
     * @brief Stores all the textures loaded so far.
     *
     * Optimization to ensure textures are not loaded more than once.
     */
    std::vector<Texture> textures_loaded;

    /**
     * @brief Stores all mesh data for the vehicle.
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

    /**
     * @brief Constructs a VehicleMeshInfo object and loads the model.
     * @param path Path to the 3D model file.
     * @param vehInfo Reference to vehicle configuration parser.
     * @param textureData Map of texture names to image parameters.
     * @param textureCache Reference to the texture cache.
     * @param gamma Enable gamma correction if true.
     */
    VehicleMeshInfo(std::string const &path, ConfigParser& vehInfo,
        const std::unordered_map<std::string, imageParam>& textureData,
        TextureCache& textureCache,
        bool gamma = false) : gammaCorrection(gamma)
    {
        loader.LoadModel(path, meshes, vehInfo, textureData, textureCache);
    }

    /**
     * @brief Returns the maximum texture ID used by this vehicle.
     * @return Maximum texture ID.
     */
    unsigned int getMaxTextureID() const {
        return maxTextureID;
    }
    // /**
    //  * @brief Draws the model and all its meshes.
    //  * @param shader Pointer to the vehicle shader.
    //  */
    // void Draw(VehicleShader *shader)
    // {
    //     for(unsigned int i = 0; i < meshes.size(); i++)
    //         meshes[i].Draw(shader);
    // }

private:
    /**
     * @brief Maximum texture ID (should be determined automatically in future).
     */
    const unsigned int maxTextureID = 6; ///< TODO: get this automatically
};