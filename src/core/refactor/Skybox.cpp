#include "Skybox.h"

#include <glad/glad.h>
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/*
 *        +y
 *    -x  +z  +x  -z
 *        -y
 * 
 */

// skybox for glass material
bool Skybox::Init(const std::vector<std::string>& faces){
    cubemap_ = LoadCubemap(faces);
    return true;
}

unsigned int Skybox::LoadCubemap(
    const std::vector<std::string>& faces) const {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data =
            stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            

            stbi_image_free(data);
        } else {
            printf("Cubemap texture failed to load at path: %s",
                   faces[i].c_str());
            stbi_image_free(data);
            assert(0);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    

    return textureID;
}

void Skybox::ActiveCubeMap() const {
    glActiveTexture(GL_TEXTURE0 + bind_point_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_);
}