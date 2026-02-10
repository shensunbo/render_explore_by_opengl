
#pragma once
#include <vector>
#include <string>
#include "gl/gl_headers.h"
#include "CommonDataStruct.h"
// #include "VehicleShader.h"

/**
 * @class BufferObjectData
 * @brief Represents a mesh with its associated OpenGL buffer objects and material data.
 *
 * This class manages vertex/index buffers, textures, and material data for a single mesh.
 * It provides methods for binding VAOs, updating uniform buffers, and initializing OpenGL resources.
 */
class BufferObjectData {
public:
    /**
     * @brief Vertex data for the mesh.
     */
    std::vector<Vertex> vertices;

    /**
     * @brief Index data for the mesh.
     */
    std::vector<unsigned int> indices;

    /**
     * @brief Textures associated with the mesh.
     */
    std::vector<Texture> textures;

    /**
     * @brief Name of the mesh.
     */
    std::string meshName;

    /**
     * @brief Material properties for the mesh.
     */
    myMaterial mMaterial;

    /**
     * @brief Uniform buffer object material data.
     */
    UboMat mUboMat{};

    /**
     * @brief OpenGL Vertex Array Object ID.
     */
    unsigned int VAO;

    /**
     * @brief Constructs a BufferObjectData object and initializes OpenGL buffers.
     * @param vertices Vertex data.
     * @param indices Index data.
     * @param textures Texture data.
     * @param mMaterial Material properties.
     * @param mName Name of the mesh.
     */
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

    /**
     * @brief Binds the VAO for this mesh.
     */
    void bindVao() const {
        glBindVertexArray(VAO);
    }

    /**
     * @brief Returns the number of indices in the mesh.
     * @return Number of indices.
     */
    size_t getIndicesSize() const {
        return indices.size();
    }

    /**
     * @brief Updates the uniform buffer object with new material data.
     * @param material Material data to upload.
     */
    void updateUbo(const UboMat& material) {
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UboMat), &material);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    /**
     * @brief Binds the uniform buffer object for this mesh.
     */
    void bindUbo() const {
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
    }

    /**
     * @brief Destructor for BufferObjectData. Releases OpenGL resources.
     */
    ~BufferObjectData();

    BufferObjectData(const BufferObjectData&) = delete;
    BufferObjectData& operator=(const BufferObjectData&) = delete;
    BufferObjectData(BufferObjectData&& other) noexcept;
    BufferObjectData& operator=(BufferObjectData&& other) noexcept;

private:
    /**
     * @brief OpenGL Vertex Buffer Object ID.
     */
    GLuint VBO{0};

    /**
     * @brief OpenGL Element Buffer Object ID.
     */
    GLuint EBO{0};

    /**
     * @brief OpenGL Uniform Buffer Object ID.
     */
    GLuint UBO{0};

    /**
     * @brief Resets OpenGL buffer handles to zero.
     */
    void resetGlHandles();

    /**
     * @brief Initializes all OpenGL buffer objects and attribute pointers for the mesh.
     */
    void setupMesh();
};