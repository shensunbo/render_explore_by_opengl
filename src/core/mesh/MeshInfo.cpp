#include "MeshInfo.h"
#include "log/mylog.h"

MeshInfo::MeshInfo(const std::string& path,
                   ConfigParser& cfgInfo,
                   const std::unordered_map<std::string, imageParam>& textureData,
                   TextureCache& textureCache,
                   bool gamma)
    : gammaCorrection_(gamma) {
    LOG_I("[MeshInfo] Loading model from path: {}", path);
    loader_.LoadModel(path, meshes_, cfgInfo, textureData, textureCache);
    LOG_I("[MeshInfo] Finished loading model from path: {}", path);
}

// Defined here so ModelLoader::~ModelLoader() is reachable (Impl is complete
// in ModelLoader.cpp; we just need ~ModelLoader() to be declared, not inline).
MeshInfo::~MeshInfo() = default;
