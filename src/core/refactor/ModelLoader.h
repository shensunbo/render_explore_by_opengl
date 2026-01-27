#pragma once
#include <vector>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

// #include "../mesh.h"
#include "BufferObjectData.h"
#include <configParser/ConfigParser.h>

class ModelLoader {
public:
    bool LoadModel(const std::string& resPath, std::vector<BufferObjectData>& meshInfo, 
            ConfigParser& vehInfo, const std::unordered_map<std::string, imageParam>& textureData);
    
    // static unsigned int ImageFromFile(std::string filename, imageParam& pngData);
private:
    void processNode(aiNode *node, const aiScene *scene, std::vector<BufferObjectData>& meshInfo, ConfigParser& vehInfo,
                    const std::unordered_map<std::string, imageParam>& textureData);
    BufferObjectData processMesh(aiMesh *mesh, const aiScene *scene, 
        const glm::mat4& translationMatrix, ConfigParser& vehInfo, 
        const std::unordered_map<std::string, imageParam>& textureData);

    myMaterial loadMaterial(aiMaterial* mat);
    unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
    unsigned int TextureFromBuffer(const std::string& path, const std::unordered_map<std::string, imageParam>& textureData);
    // unsigned int ImageFromFile(std::string filename, imageParam& pngData);
    unsigned int TextureFromKTXFile(const char *path, const std::string &directory);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type,
                                          const char* typeName,
                                          std::string meshName,
                                          ConfigParser& vehInfo,
                                          const std::unordered_map<std::string, imageParam>& textureData);

    glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
        glm::mat4 to;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                to[i][j] =
                    from[j][i];  // Transpose matrix by swapping rows and columns
            }
        }
        return to;
    }

    // std::vector<Texture> textures_loaded;
    std::vector<Texture> m_textures_loaded;
    std::string directory;
    // std::unordered_map<std::string, imageParam> m_loaded_texture_data;
};