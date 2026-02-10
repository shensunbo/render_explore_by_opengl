
#pragma once
#include <vector>
#include <string>
#include "gl/gl_headers.h"
#include "CommonDataStruct.h"
// #include "VehicleShader.h"

class BufferObjectData {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    std::string meshName;
    myMaterial mMaterial;
    UboMat mUboMat{};
    unsigned int VAO;


    // constructor
    BufferObjectData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
        std::vector<Texture> textures, myMaterial mMaterial, std::string mName)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->mMaterial = mMaterial;
        this->meshName = mName;

        this->mUboMat.diffuseColor = glm::vec4(mMaterial.diffuseColor, 1.0f);
        this->mUboMat.specularColor = glm::vec4(mMaterial.specularColor, 1.0f);
        this->mUboMat.shininess = mMaterial.shininess;
        this->mUboMat.shininessStrength = mMaterial.ShininessStrength;

    for (const auto& tex : textures) {
        if (tex.type == "texture_diffuse") {
            this->mUboMat.texture_diffuse_load = 1;
        } else if (tex.type == "texture_specular") {
            this->mUboMat.texture_specular_load = 1;
        } else if (tex.type == "texture_normal") {
            this->mUboMat.texture_normal_load = 1;
        } else if (tex.type == "texture_ao") {
            this->mUboMat.texture_ao_load = 1;
        } else if (tex.type == "texture_alpha") {
            this->mUboMat.texture_alpha_load = 1;
        } else if (tex.type == "texture_roughness") {
            this->mUboMat.texture_roughness_load = 1;
        } else if (tex.type == "texture_metallic") {
            this->mUboMat.texture_metallic_load = 1;
        } else if (tex.type == "texture_emissive") {
            this->mUboMat.texture_emissive_load = 1;
        }
    }

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    void bindVao(){
        glBindVertexArray(VAO);
    }

    size_t getIndicesSize(){
        return indices.size();
    }

    void updateUbo(const UboMat& material) {
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UboMat), &material);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void bindUbo() {
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
    }

    ~BufferObjectData();

    BufferObjectData(const BufferObjectData&) = delete;
    BufferObjectData& operator=(const BufferObjectData&) = delete;
    BufferObjectData(BufferObjectData&& other) noexcept;
    BufferObjectData& operator=(BufferObjectData&& other) noexcept;

private:
    // render data 
    GLuint VBO{0};
    GLuint EBO{0};
    GLuint UBO{0};

    void resetGlHandles();

    // initializes all the buffer objects/arrays
    void setupMesh();
};