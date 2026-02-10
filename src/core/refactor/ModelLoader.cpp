#include "ModelLoader.h"
#include "gl/gl_headers.h"
#include <stb_image.h>
#include <ktx.h>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>

#include "BufferObjectData.h"
#include "log/mylog.h"
#include "VehicleShader.h"
#include "TextureCache.h"

bool ModelLoader::LoadModel(const std::string& resPath, std::vector<BufferObjectData>& meshInfo, 
    ConfigParser& vehInfo, const std::unordered_map<std::string, imageParam>& textureData,
    TextureCache& textureCache){
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(resPath, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs | aiProcess_ValidateDataStructure);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        mylog(LogLevel::E,"ERROR::ASSIMP:: %s, ", importer.GetErrorString());
        return false;
    }

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, meshInfo, vehInfo, textureData, textureCache);

    return true;
}

 // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void ModelLoader::processNode(aiNode *node, const aiScene *scene, std::vector<BufferObjectData>& meshInfo, 
    ConfigParser& vehInfo, const std::unordered_map<std::string, imageParam>& textureData, TextureCache& textureCache)
{
    aiMatrix4x4 mTransformation = node->mTransformation;

    aiNode* parent = node->mParent;
    while (parent) {
        mTransformation = parent->mTransformation * mTransformation;
        parent = parent->mParent;
    }

    glm::mat4 glmTranslationMatrix = aiMatrix4x4ToGlm(mTransformation);

    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshInfo.push_back(processMesh(mesh, scene, glmTranslationMatrix, vehInfo, textureData, textureCache));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
    processNode(node->mChildren[i], scene, meshInfo, vehInfo, textureData, textureCache);
    }

}

BufferObjectData ModelLoader::processMesh(aiMesh *mesh, const aiScene *scene, const glm::mat4& translationMatrix, 
    ConfigParser& vehInfo, const std::unordered_map<std::string, imageParam>& textureData, TextureCache& textureCache)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // glm::mat4 glmTranslationMatrix = aiMatrix4x4ToGlm(translationMatrix);

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
            glm::vec4(vertex.Position, 1.0f);  // Convert to homogeneous coordinates.
        position = translationMatrix * position;

        // Update vertex position after transform.
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
        auto start_time = std::chrono::high_resolution_clock::now();

        std::vector<Texture> diffuseMaps =
            LoadTextures(material, aiTextureType_DIFFUSE, "texture_diffuse",
                         std::string(mesh->mName.C_Str()), vehInfo, textureData, textureCache);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps =
            LoadTextures(material, aiTextureType_SPECULAR, "texture_specular",
                         std::string(mesh->mName.C_Str()), vehInfo, textureData, textureCache);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps =
            LoadTextures(material, aiTextureType_NORMALS, "texture_normal",
                         std::string(mesh->mName.C_Str()), vehInfo, textureData, textureCache);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> aoMaps =
            LoadTextures(material, aiTextureType_AMBIENT_OCCLUSION, "texture_ao",
                         std::string(mesh->mName.C_Str()), vehInfo, textureData, textureCache);
        textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

        std::vector<Texture> alphaMaps =
            LoadTextures(material, aiTextureType_OPACITY, "texture_alpha",
                         std::string(mesh->mName.C_Str()), vehInfo, textureData, textureCache);
        textures.insert(textures.end(), alphaMaps.begin(), alphaMaps.end());

        std::vector<Texture> roughnessMaps =
            LoadTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness",
                         std::string(mesh->mName.C_Str()), vehInfo, textureData, textureCache);
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

        std::vector<Texture> metallicMaps =
            LoadTextures(material, aiTextureType_METALNESS, "texture_metallic",
                         std::string(mesh->mName.C_Str()), vehInfo, textureData, textureCache);
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

        auto end_time = std::chrono::high_resolution_clock::now();
        auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        mylog(LogLevel::I, "[ModelLoader] mesh %s material %s texture load cost: %lld ms", mesh->mName.C_Str(), material->GetName().C_Str(), cost);
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
    //         // Use Phong shading model.
    //     } else if (shadingModel == aiShadingMode_Gouraud) {
    //         // Use Gouraud shading model.
    //     }
    // }

    material.MaterialName = std::string(mat->GetName().C_Str());

    return material;
}

unsigned int ModelLoader::TextureFromBuffer(const std::string& path, const std::unordered_map<std::string, imageParam>& textureData){
    // Retrieve preloaded texture data from the in-memory map.
    auto it = textureData.find(path);
    if (it == textureData.end()) {
        mylog(LogLevel::E, "Texture data not found in buffer: %s", path.c_str());
        assert(false);
        return 0;
    }

    const imageParam& imgData = it->second;
    
    // Ensure the texture payload is valid.
    if (!imgData.data) {
        mylog(LogLevel::E, "Texture data is null for path: %s", path.c_str());
        assert(false);
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    GLenum format;
    if (imgData.nrChannels == 1)
        format = GL_RED;
    else if (imgData.nrChannels == 3)
        format = GL_RGB;
    else if (imgData.nrChannels == 4)
        format = GL_RGBA;
    else {
        mylog(LogLevel::E, "Unsupported channel count %d for texture: %s", imgData.nrChannels, path.c_str());
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, imgData.width, imgData.height, 0, format, GL_UNSIGNED_BYTE, imgData.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mylog(LogLevel::I, "Texture created from buffer: %s, %dx%d, channels: %d", path.c_str(), imgData.width, imgData.height, imgData.nrChannels);

    return textureID;
}

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
    // TODO: Shader path cannot distinguish single-channel from multi-channel textures.
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

unsigned int ModelLoader::TextureFromKTXFile(const char *path, const std::string &directory)
{
    auto total_start = std::chrono::high_resolution_clock::now();
    
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    mylog(LogLevel::I, "Loading KTX texture: %s", filename.c_str());
    
    auto load_start = std::chrono::high_resolution_clock::now();
    ktxTexture* texture = nullptr;
    KTX_error_code result = ktxTexture_CreateFromNamedFile(filename.c_str(),
                                                            KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                                            &texture);
    auto load_end = std::chrono::high_resolution_clock::now();
    auto load_cost = std::chrono::duration_cast<std::chrono::milliseconds>(load_end - load_start).count();
    
    if (result != KTX_SUCCESS) {
        mylog(LogLevel::E, "Failed to load KTX file: %s, error: %s", filename.c_str(), ktxErrorString(result));
        return 0;
    }
    
    mylog(LogLevel::I, "KTX file read cost: %lld ms - Dimensions: %dx%d, Levels: %d, Layers: %d, Faces: %d", 
          load_cost, texture->baseWidth, texture->baseHeight, texture->numLevels, texture->numLayers, texture->numFaces);
    
    GLuint textureID = 0;  // Initialize texture handle.
    GLenum target = 0;     // Initialize target placeholder.
    GLenum glerror = GL_NO_ERROR;  // Initialize GL error placeholder.
    
    // Upload to OpenGL; ktxTexture_GLUpload creates the texture and fills data.
    auto upload_start = std::chrono::high_resolution_clock::now();
    result = ktxTexture_GLUpload(texture, &textureID, &target, &glerror);
    auto upload_end = std::chrono::high_resolution_clock::now();
    auto upload_cost = std::chrono::duration_cast<std::chrono::milliseconds>(upload_end - upload_start).count();
    
    if (result != KTX_SUCCESS) {
        mylog(LogLevel::E, "Failed to upload KTX texture to OpenGL: %s", ktxErrorString(result));
        if (glerror != GL_NO_ERROR) {
            mylog(LogLevel::E, "OpenGL error during upload: 0x%x", glerror);
        }
        ktxTexture_Destroy(texture);
        return 0;
    }
    
    // Check for GL errors reported by ktxTexture_GLUpload.
    if (glerror != GL_NO_ERROR) {
        mylog(LogLevel::E, "GL error reported by ktxTexture_GLUpload: 0x%x", glerror);
        ktxTexture_Destroy(texture);
        return 0;
    }
    
    mylog(LogLevel::I, "KTX GPU upload cost: %lld ms - Texture ID: %u, Target: 0x%x", upload_cost, textureID, target);
    
    // Validate the returned texture ID.
    GLboolean isTexture = glIsTexture(textureID);
    if (!isTexture) {
        mylog(LogLevel::E, "ktxTexture_GLUpload returned invalid texture ID: %u", textureID);
        ktxTexture_Destroy(texture);
        return 0;
    }
    
    // Inspect GL error state after upload.
    GLenum err1 = glGetError();
    if (err1 != GL_NO_ERROR) {
        mylog(LogLevel::E, "GL error after ktxTexture_GLUpload: 0x%x", err1);
    }
    
    // Configure texture sampling parameters.
    auto param_start = std::chrono::high_resolution_clock::now();
    glBindTexture(target, textureID);
    GLenum err2 = glGetError();
    if (err2 != GL_NO_ERROR) {
        mylog(LogLevel::E, "GL error after glBindTexture: 0x%x", err2);
    }
    
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLenum err3 = glGetError();
    if (err3 != GL_NO_ERROR) {
        mylog(LogLevel::E, "GL error after glTexParameteri: 0x%x", err3);
    }
    
    glBindTexture(target, 0);
    auto param_end = std::chrono::high_resolution_clock::now();
    auto param_cost = std::chrono::duration_cast<std::chrono::milliseconds>(param_end - param_start).count();

    ktxTexture_Destroy(texture);

    CHECK_GLES_STATUS();
    
    auto total_end = std::chrono::high_resolution_clock::now();
    auto total_cost = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();
    
    mylog(LogLevel::I, "KTX total cost: %lld ms (read: %lld, upload: %lld, param: %lld)", 
          total_cost, load_cost, upload_cost, param_cost);
    
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
                                                    ConfigParser& vehInfo,
                                                    const std::unordered_map<std::string, imageParam>& textureData,
                                                    TextureCache& textureCache) {
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
            
            // Check the file extension to decide the loading path (legacy logic retained for reference).
            // std::string texPath = textureName;
            // bool isKTX = false;
            // if (texPath.length() >= 4) {
            //     std::string ext = texPath.substr(texPath.length() - 4);
            //     if (ext == ".ktx" || ext == ".KTX") {
            //         isKTX = true;
            //     } else if (texPath.length() >= 5) {
            //         ext = texPath.substr(texPath.length() - 5);
            //         if (ext == ".ktx2" || ext == ".KTX2") {
            //             isKTX = true;
            //         }
            //     }
            // }
            
            // if (isKTX) {
            //     texture.id = TextureFromKTXFile(textureName.c_str(), directory);
            //     mylog(LogLevel::I, "Loaded KTX texture: %s", textureName.c_str());
            // } else {
            //     texture.id = TextureFromFile(textureName.c_str(), directory);
            // }

            auto texIt = textureData.find(textureName);
            if (texIt == textureData.end()) {
                mylog(LogLevel::W, "Texture data missing for %s", textureName.c_str());
                texture.id = 0;
            } else {
                texture.id = textureCache.getOrCreate(textureName, texIt->second);
            }
            if(texture.id == 0){
                mylog(LogLevel::E, "Failed to load texture: %s for mesh %s ", textureName.c_str(),
                       meshName.c_str());
                assert(false);
            }
            texture.bindId = texId;
            texture.type = typeName;
            texture.path = textureName;
            textures.push_back(texture);
            m_textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }

    return textures;
}