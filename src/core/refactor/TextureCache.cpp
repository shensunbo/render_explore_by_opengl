#include "TextureCache.h"

TextureCache::~TextureCache(){
    destroy();
}

GLuint TextureCache::getOrCreate(const std::string& path, const imageParam& imgData, bool srgb){
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second;
    }

    if (!imgData.data) {
        return 0;
    }

    GLuint textureID = 0;
    glGenTextures(1, &textureID);

    GLenum format = GL_RGB;
    if (imgData.nrChannels == 1) format = GL_RED;
    else if (imgData.nrChannels == 3) format = GL_RGB;
    else if (imgData.nrChannels == 4) format = GL_RGBA;

    GLint internalFormat = static_cast<GLint>(format);
    if (srgb) {
        if (imgData.nrChannels == 3) internalFormat = GL_SRGB8;
        else if (imgData.nrChannels == 4) internalFormat = GL_SRGB8_ALPHA8;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imgData.width, imgData.height, 0, format, GL_UNSIGNED_BYTE, imgData.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textures_[path] = textureID;
    return textureID;
}

void TextureCache::destroy(){
    for (auto& kv : textures_) {
        if (kv.second != 0) {
            glDeleteTextures(1, &kv.second);
        }
    }
    textures_.clear();
}
