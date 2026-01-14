#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

// #define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	// //bone indexes which will influence this vertex
	// int m_BoneIDs[MAX_BONE_INFLUENCE];
	// //weights from each bone
	// float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    unsigned int bindId; // texture slot
    std::string type;
    std::string path;
};

struct myMaterial {
    unsigned int MaterialIndex;
    std::string  MaterialName;
    unsigned int shadingModel;
    float shininess;
    float Opacity;
    float TransparencyFactor;
    float ShininessStrength;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    glm::vec3 ambientColor;
    glm::vec3 TransparentColor;
};

struct UboMat {
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    float opacity;
    float shininess;
    float shininessStrength;
    // use uint32_t to align with glsl bool
    uint32_t texture_diffuse_load;
    uint32_t texture_specular_load;
    uint32_t texture_normal_load;
    uint32_t texture_ao_load;
    uint32_t texture_alpha_load;
    uint32_t texture_roughness_load;
    uint32_t texture_metallic_load;
    uint32_t texture_emissive_load;
};

struct imageParam{
    int width;
    int height;
    int nrChannels;
    unsigned char *data;
};