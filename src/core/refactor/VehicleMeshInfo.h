#pragma once
// #include "../mesh.h"
// #include "../shader.h"
#include "VehicleShader.h"
#include "log/mylog.h"
#include "ModelLoader.h"
#include "CommonDataStruct.h"
#include "BufferObjectData.h"

class VehicleMeshInfo {
public:
    // model data 
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<BufferObjectData>    meshes;
    bool gammaCorrection;
    ModelLoader loader;

    // constructor, expects a filepath to a 3D model.
    VehicleMeshInfo(std::string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loader.LoadModel(path, meshes);
    }

    // draws the model, and thus all its meshes
    void Draw(VehicleShader &shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
};