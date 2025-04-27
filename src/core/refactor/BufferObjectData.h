
#pragma once
#include <vector>
#include <string>

#include "CommonDataStruct.h"
#include "VehicleShader.h"

class BufferObjectData {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;
    myMaterial mMaterial;

    // constructor
    BufferObjectData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
        std::vector<Texture> textures, myMaterial mMaterial)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->mMaterial = mMaterial;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    void bindVao(){
        glBindVertexArray(VAO);
    }

    size_t getIndicesSize(){
        return indices.size();
    }

    // render the mesh
    // void Draw(VehicleShader *shader);

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};