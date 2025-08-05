#include "ModelLoader.h"
#include <glad/glad.h> 
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
#include <iostream>

#include "BufferObjectData.h"
#include "log/mylog.h"

bool ModelLoader::LoadModel(const std::string& resPath, std::vector<BufferObjectData>& meshInfo, ConfigParser& vehInfo){
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(resPath, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs | aiProcess_ValidateDataStructure);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        mylog(LogLevel::E,"ERROR::ASSIMP:: %s, ", importer.GetErrorString());
        return false;
    }
    // retrieve the directory path of the filepath
    directory = resPath.substr(0, resPath.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, meshInfo, vehInfo);

    return true;
}

 // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void ModelLoader::processNode(aiNode *node, const aiScene *scene, std::vector<BufferObjectData>& meshInfo, ConfigParser& vehInfo)
{
    aiMatrix4x4 mTransformation = node->mTransformation;

    aiNode* parent = node->mParent;
    while (parent) {
        mTransformation = parent->mTransformation * mTransformation;
        parent = parent->mParent;
    }

    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshInfo.push_back(processMesh(mesh, scene, mTransformation, vehInfo));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, meshInfo, vehInfo);
    }

}

BufferObjectData ModelLoader::processMesh(aiMesh *mesh, const aiScene *scene, const aiMatrix4x4& translationMatrix, 
    ConfigParser& vehInfo)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    glm::mat4 glmTranslationMatrix = aiMatrix4x4ToGlm(translationMatrix);

    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
           
        }
        else{
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->HasTangentsAndBitangents()) {
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        } else {
            // tangent
            vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            // bitangent
            vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
            mylog(LogLevel::I,"%s No TangentsAndBitangents", mesh->mName.C_Str());
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < vertices.size(); i++) {
        Vertex& vertex = vertices[i];

        glm::vec4 position =
            glm::vec4(vertex.Position, 1.0f);  // 转换为齐次坐标
        position = glmTranslationMatrix * position;

        // 更新顶点的位置
        vertex.Position = glm::vec3(position.x, position.y, position.z);
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    

    // texture_diffuse:  0
    // texture_specular: 1
    // texture_normal:   2
    // texture_ao:       3
    // texture_alpha:    4
    // texture_roughness:5
    // texture_metallic: 6
    if(vehInfo.needTexture(std::string(mesh->mName.C_Str()), material->GetName().C_Str())){
        std::vector<Texture> diffuseMaps =
            LoadTextures(material, aiTextureType_DIFFUSE, "texture_diffuse",
                         std::string(mesh->mName.C_Str()), vehInfo);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps =
            LoadTextures(material, aiTextureType_SPECULAR, "texture_specular",
                         std::string(mesh->mName.C_Str()), vehInfo);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps =
            LoadTextures(material, aiTextureType_NORMALS, "texture_normal",
                         std::string(mesh->mName.C_Str()), vehInfo);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> aoMaps =
            LoadTextures(material, aiTextureType_AMBIENT_OCCLUSION, "texture_ao",
                         std::string(mesh->mName.C_Str()), vehInfo);
        textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

        std::vector<Texture> alphaMaps =
            LoadTextures(material, aiTextureType_OPACITY, "texture_alpha",
                         std::string(mesh->mName.C_Str()), vehInfo);
        textures.insert(textures.end(), alphaMaps.begin(), alphaMaps.end());

        std::vector<Texture> roughnessMaps =
            LoadTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness",
                         std::string(mesh->mName.C_Str()), vehInfo);
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

        std::vector<Texture> metallicMaps =
            LoadTextures(material, aiTextureType_METALNESS, "texture_metallic",
                         std::string(mesh->mName.C_Str()), vehInfo);
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
    }

    myMaterial mMaterial;
    mMaterial = loadMaterial(material);
    mMaterial.MaterialIndex = mesh->mMaterialIndex;

    std::string meshName = std::string(mesh->mName.C_Str());
    
    // return a mesh object created from the extracted mesh data
    return BufferObjectData(vertices, indices, textures, mMaterial, meshName);
}

myMaterial ModelLoader::loadMaterial(aiMaterial* mat)
{
    myMaterial material = {};

    aiColor3D diffuseColor;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor)) {
        material.diffuseColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    } else {
        std::cout << "Failed to get diffuse color" << std::endl;
    }

    aiColor3D specularColor;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor)) {
    material.specularColor = glm::vec3(specularColor.r, specularColor.g, specularColor.b);
    } else {
        std::cout << "Failed to get specular color" << std::endl;
    }

    aiColor3D ambientColor;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor)) {
        material.ambientColor = glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);
    } else {
        std::cout << "Failed to get ambient color" << std::endl;
    }

    float shininess;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, shininess)) {
    material.shininess = shininess;
    } else {
        std::cout << "Failed to get shininess" << std::endl;
    }

    float opacity = 1.0f;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_OPACITY, opacity)) {
        material.Opacity = opacity;
    }

    float transparencyFactor = 1.0f;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparencyFactor)) {
        material.TransparencyFactor = 1.0f - transparencyFactor;
    }

    float shininessStrength = 1.0f;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength)) {
        material.ShininessStrength = 1.0f - shininessStrength;
    }

    aiColor3D transparentColor;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_TRANSPARENT, transparentColor)) {
        material.TransparentColor = glm::vec3(transparentColor.r, transparentColor.g, transparentColor.b);
    }

    // int shadingModel;
    // if (AI_SUCCESS == mat->Get(AI_MATKEY_SHADING_MODEL, shadingModel)) {
    //     if (shadingModel == aiShadingMode_Phong) {
    //         // 设置为 Phong 着色模型
    //     } else if (shadingModel == aiShadingMode_Gouraud) {
    //         // 设置为 Gouraud 着色模型
    //     }
    // }

    material.MaterialName = std::string(mat->GetName().C_Str());

    return material;
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
// std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
// {
//     std::vector<Texture> textures;
//     for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//     {
//         aiString str;
//         mat->GetTexture(type, i, &str);
//         // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//         bool skip = false;
//         for(unsigned int j = 0; j < textures_loaded.size(); j++)
//         {
//             if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
//             {
//                 textures.push_back(textures_loaded[j]);
//                 skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
//                 break;
//             }
//         }
//         if(!skip)
//         {   // if texture hasn't been loaded already, load it
//             Texture texture;
//             texture.id = TextureFromFile(str.C_Str(), directory);
//             texture.type = typeName;
//             texture.path = str.C_Str();
//             textures.push_back(texture);
//             textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
//         }
//     }
//     return textures;
// }

unsigned int ModelLoader::TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        // TODO: 着色器中无法区分纹理是单通道还是多通道
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        mylog(LogLevel::I, "Texture loaded at path: %s, channels: %d", path, nrComponents);
    }
    else
    {
        mylog(LogLevel::E, "Texture failed to load at path: %s", path);
        stbi_image_free(data);
    }

    return textureID;
}

/**
 * Load textures when the model file does not contain any textures
 *
 * Texture types:
 *   - 0: texture_diffuse
 *   - 1: texture_specular
 *   - 2: texture_normal
 *   - 3: texture_ao
 *   - 4: texture_alpha
 *   - 5: texture_roughness
 *   - 6: texture_metallic
 *
 * @note typename will be used to bind the texture to the shader
 */
std::vector<Texture> ModelLoader::LoadTextures(aiMaterial* mat,
                                                    aiTextureType type,
                                                    const char* typeName,
                                                    std::string meshName,
                                                    ConfigParser& vehInfo) {
    std::vector<Texture> textures;
    bool skip = false;
    std::string INVALID_TEXTURE_NAME = "";
    std::string textureName = INVALID_TEXTURE_NAME;
    unsigned int texId = 0;
    switch (type) {
        case aiTextureType_DIFFUSE:
            texId = 0;
            textureName =
                vehInfo.getTextureData(meshName, mat->GetName().C_Str()).diffuse;
            break;
        case aiTextureType_SPECULAR:
            texId = 1;
            textureName =
                vehInfo.getTextureData(meshName, mat->GetName().C_Str()).specular;
            break;
        case aiTextureType_NORMALS:
            texId = 2;
            textureName =
                vehInfo.getTextureData(meshName, mat->GetName().C_Str()).normal;
            break;
        case aiTextureType_AMBIENT_OCCLUSION:
            texId = 3;
            textureName =
                vehInfo.getTextureData(meshName, mat->GetName().C_Str()).ao;
            break;
        case aiTextureType_OPACITY:
            texId = 4;
            textureName =
                vehInfo.getTextureData(meshName, mat->GetName().C_Str()).alpha;
            break;
        case aiTextureType_DIFFUSE_ROUGHNESS:
            texId = 5;
            textureName =
                vehInfo.getTextureData(meshName, mat->GetName().C_Str()).roughness;
            break;
        case aiTextureType_METALNESS:
            texId = 6;
            textureName =
                vehInfo.getTextureData(meshName, mat->GetName().C_Str()).metallic;
            break;
        default:
            textureName = INVALID_TEXTURE_NAME;
            mylog(LogLevel::E, "couldn't load typeName %s for mesh %s ", typeName,
                   meshName.c_str());
            break;
    }

    if (textureName != INVALID_TEXTURE_NAME) {
        for (unsigned int j = 0; j < m_textures_loaded.size(); j++) {
            if (std::strcmp(m_textures_loaded[j].path.data(),
                            textureName.c_str()) == 0) {
                textures.push_back(m_textures_loaded[j]);
                skip =
                    true;  // a texture with the same filepath has already been
                           // loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(textureName.c_str(), directory);
            texture.bindId = texId;
            texture.type = typeName;
            texture.path = textureName;
            textures.push_back(texture);
            m_textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }

    return textures;
}