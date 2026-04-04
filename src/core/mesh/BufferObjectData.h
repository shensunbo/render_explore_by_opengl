
#pragma once
#include <vector>
#include <string>
#include "gl/gl_headers.h"
#include "common/CommonDataStruct.h"
// #include "Shader.h"

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
     * @brief Constructs a BufferObjectData object and initializes OpenGL buffers.
     * @param vertices Vertex data.
     * @param indices Index data.
     * @param textures Texture data.
     * @param mMaterial Material properties.
     * @param mName Name of the mesh.
     */
    BufferObjectData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
        std::vector<Texture> textures, myMaterial mMaterial, std::string mName);
    
    /**
     * @brief Destructor for BufferObjectData. Releases OpenGL resources.
     */
    ~BufferObjectData();

    BufferObjectData(const BufferObjectData&) = delete;
    BufferObjectData& operator=(const BufferObjectData&) = delete;
    BufferObjectData(BufferObjectData&& other) noexcept;
    BufferObjectData& operator=(BufferObjectData&& other) noexcept;

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
     * @brief Updates the per-instance model matrix buffer for instanced rendering.
     * @param matrices Vector of model matrices, one per instance.
     */
    void updateInstanceBuffer(const std::vector<glm::mat4>& matrices) const;

private:
    void resetGlHandles();

    /**
     * @brief Initializes all OpenGL buffer objects and attribute pointers for the mesh.
     */
    void setupMesh();

public:
    unsigned int VAO{0};

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    std::string meshName;
    myMaterial mMaterial;
    UboMat mUboMat{};

private:

    GLuint VBO{0};
    GLuint EBO{0};
    GLuint UBO{0};
    GLuint instanceVBO{0}; ///< Per-instance model matrix buffer for instanced rendering.
};