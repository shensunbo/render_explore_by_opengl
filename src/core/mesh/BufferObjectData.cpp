#include "BufferObjectData.h"

BufferObjectData::BufferObjectData(std::vector<Vertex> vertices,
    std::vector<unsigned int> indices,
    std::vector<Texture> textures,
    myMaterial mMaterial,
    std::string mName)
        : vertices(std::move(vertices)),
            indices(std::move(indices)),
            textures(std::move(textures)),
            meshName(std::move(mName)),
            mMaterial(std::move(mMaterial))
{
    // Zero-initialize UBO struct to ensure all flags/values are deterministic.
    mUboMat = {};

    // Fill material-derived fields (POD members are safe after move).
    mUboMat.diffuseColor = glm::vec4(mMaterial.diffuseColor, 1.0f);
    mUboMat.specularColor = glm::vec4(mMaterial.specularColor, 1.0f);
    mUboMat.shininess = mMaterial.shininess;
    mUboMat.shininessStrength = mMaterial.ShininessStrength;

    // Set texture-load flags based on moved-in textures member.
    for (const auto& tex : this->textures) {
        const auto& t = tex.type;
        if (t == "texture_diffuse") mUboMat.texture_diffuse_load = 1;
        else if (t == "texture_specular") mUboMat.texture_specular_load = 1;
        else if (t == "texture_normal") mUboMat.texture_normal_load = 1;
        else if (t == "texture_ao") mUboMat.texture_ao_load = 1;
        else if (t == "texture_alpha") mUboMat.texture_alpha_load = 1;
        else if (t == "texture_roughness") mUboMat.texture_roughness_load = 1;
        else if (t == "texture_metallic") mUboMat.texture_metallic_load = 1;
        else if (t == "texture_emissive") mUboMat.texture_emissive_load = 1;
    }

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

BufferObjectData::~BufferObjectData() {
    resetGlHandles();
}

BufferObjectData::BufferObjectData(BufferObjectData&& other) noexcept {
    *this = std::move(other);
}

BufferObjectData& BufferObjectData::operator=(BufferObjectData&& other) noexcept {
    if (this != &other) {
        resetGlHandles();

        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        textures = std::move(other.textures);
        meshName = std::move(other.meshName);
        mMaterial = other.mMaterial;
        mUboMat = other.mUboMat;

        VAO = other.VAO; other.VAO = 0;
        VBO = other.VBO; other.VBO = 0;
        EBO = other.EBO; other.EBO = 0;
        UBO = other.UBO; other.UBO = 0;
        instanceVBO = other.instanceVBO; other.instanceVBO = 0;
    }
    return *this;
}

void BufferObjectData::resetGlHandles(){
    if (instanceVBO) {
        glDeleteBuffers(1, &instanceVBO);
        instanceVBO = 0;
    }
    if (UBO) {
        glDeleteBuffers(1, &UBO);
        UBO = 0;
    }
    if (EBO) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

void BufferObjectData::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // // ids
    // glEnableVertexAttribArray(5);
    // glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // // weights
    // glEnableVertexAttribArray(6);
    // glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    // glBindVertexArray(0);

    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(UboMat), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);

    // Setup per-instance model matrix buffer (mat4 = 4 vec4s at locations 5-8).
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    for (int col = 0; col < 4; ++col) {
        glEnableVertexAttribArray(5 + col);
        glVertexAttribPointer(5 + col, 4, GL_FLOAT, GL_FALSE,
                              sizeof(glm::mat4),
                              reinterpret_cast<void*>(sizeof(glm::vec4) * col));
        glVertexAttribDivisor(5 + col, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BufferObjectData::updateInstanceBuffer(const std::vector<glm::mat4>& matrices) const {
    if (matrices.empty() || !instanceVBO) return;
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(matrices.size() * sizeof(glm::mat4)),
                 matrices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}