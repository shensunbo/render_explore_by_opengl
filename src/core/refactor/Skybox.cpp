#include "Skybox.h"

#include "gl/gl_headers.h"
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/*
 *        +y
 *    -x  +z  +x  -z
 *        -y
 * 
 */

// Skybox for glass material.
bool Skybox::Init(const std::vector<std::string>& faces, const std::unordered_map<std::string, imageParam>& textureData){
    cubemap_ = LoadCubemap(faces, textureData);

    initSkybox();
    return true;
}

Skybox::~Skybox(){
    if (cubemap_) {
        glDeleteTextures(1, &cubemap_);
        cubemap_ = 0;
    }
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
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

unsigned int Skybox::LoadCubemap(const std::vector<std::string>& faces, 
        const std::unordered_map<std::string, imageParam>& textureData) const{
            
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    CHECK_GLES_STATUS();

    // int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++) {
        // unsigned char* data =
        //     stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);

    // Read texture data from preloaded buffer map.
        auto it = textureData.find(faces[i]);
        if (it == textureData.end()) {
            mylog(LogLevel::E, "Texture data not found in buffer: %s", faces[i].c_str());
            assert(false);
            return 0;
        }

        const imageParam& imgData = it->second;
        
    // // Validate that texture data exists.
        // if (!imgData.data) {
        //     SLOG_E("Texture data is null for path: {}", faces[i].c_str());
        //     assert(false);
        //     return 0;
        // }

        if (imgData.data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imgData.width,
                         imgData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData.data);
            CHECK_GLES_STATUS();

            // stbi_image_free(data);
        } else {
            mylog(LogLevel::E, "Texture data is null for path: %s", faces[i].c_str());
            assert(false);
            return 0;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    CHECK_GLES_STATUS();

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    CHECK_GLES_STATUS();

    return textureID;
}

void Skybox::ActiveCubeMap() const {
    glActiveTexture(GL_TEXTURE0 + bind_point_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_);
}

void Skybox::initSkybox(){
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    shader_.setInt("skyboxTexture",  bind_point_);
    
    mylog(LogLevel::I, "Skybox created");
}
void Skybox::drawSkybox(){
    shader_.use();
    glDepthFunc(GL_LEQUAL);// Change depth function so depth test passes when values are equal to depth buffer's content.
    glBindVertexArray(vao_);
    ActiveCubeMap();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // Restore default depth function.
}