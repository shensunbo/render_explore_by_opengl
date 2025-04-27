#pragma once
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

// #include "../mesh.h"
#include "BufferObjectData.h"

class ModelLoader {
public:
    bool LoadModel(const std::string& resPath, std::vector<BufferObjectData>& meshInfo);

private:
    void processNode(aiNode *node, const aiScene *scene, std::vector<BufferObjectData>& meshInfo);
    BufferObjectData processMesh(aiMesh *mesh, const aiScene *scene, const aiMatrix4x4& translationMatrix);
    myMaterial loadMaterial(aiMaterial* mat);
    unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

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

    std::vector<Texture> textures_loaded;
    std::string directory;
};