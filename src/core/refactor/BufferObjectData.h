
#pragma once
#include <vector>
#include <string>
#include <glad/glad.h> // holds all OpenGL type declarations
#include "CommonDataStruct.h"
// #include "VehicleShader.h"

class BufferObjectData {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;
    myMaterial mMaterial;
    UboMat mUboMat;

    // constructor
    BufferObjectData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
        std::vector<Texture> textures, myMaterial mMaterial)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->mMaterial = mMaterial;

        this->mUboMat.diffuseColor = glm::vec4(mMaterial.diffuseColor, 1.0f);
        this->mUboMat.specularColor = glm::vec4(mMaterial.specularColor, 1.0f);
        this->mUboMat.shininess = mMaterial.shininess;
        this->mUboMat.shininessStrength = mMaterial.ShininessStrength;

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

private:
    // render data 
    GLuint VBO;
    GLuint EBO;
    GLuint UBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};