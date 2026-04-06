#include "TextureCache.h"
#include "log/mylog.h"

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

    // Release any unconsumed pre-loaded KTX textures.
    std::lock_guard<std::mutex> lock(pendingMutex_);
    for (auto& kv : pendingKtx_) {
        if (kv.second) {
            ktxTexture_Destroy(kv.second);
        }
    }
    pendingKtx_.clear();
}

// ---------------------------------------------------------------------------
// preloadKtx — CPU-only: file I/O + Basis transcode. Thread-safe.
// ---------------------------------------------------------------------------
void TextureCache::preloadKtx(const std::string& path) {
    ktxTexture* ktxTex = nullptr;
    KTX_error_code result = ktxTexture_CreateFromNamedFile(
        path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex);
    if (result != KTX_SUCCESS) {
        LOG_E("KTX preload failed: {} — {}", path, ktxErrorString(result));
        return;
    }

    // Transcode UASTC/ETC1S to a GPU-native format (CPU only, no GL).
    if (ktxTex->classId == ktxTexture2_c) {
        ktxTexture2* ktxTex2 = reinterpret_cast<ktxTexture2*>(ktxTex);
        if (ktxTexture2_NeedsTranscoding(ktxTex2)) {
            result = ktxTexture2_TranscodeBasis(ktxTex2, KTX_TTF_BC7_RGBA, 0);
            if (result != KTX_SUCCESS) {
                LOG_W("BC7 transcode failed for {}, retrying as RGBA32: {}", path, ktxErrorString(result));
                result = ktxTexture2_TranscodeBasis(ktxTex2, KTX_TTF_RGBA32, 0);
            }
            if (result != KTX_SUCCESS) {
                LOG_E("KTX transcode failed: {} — {}", path, ktxErrorString(result));
                ktxTexture_Destroy(ktxTex);
                return;
            }
        }
    }

    std::lock_guard<std::mutex> lock(pendingMutex_);
    pendingKtx_[path] = ktxTex;
}

// ---------------------------------------------------------------------------
// getOrCreateKtx — GL upload (main thread). Consumes pre-loaded data if available.
// ---------------------------------------------------------------------------
GLuint TextureCache::getOrCreateKtx(const std::string& path) {
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second;
    }

    // Try to pick up pre-loaded data from preloadKtx().
    ktxTexture* ktxTex = nullptr;
    {
        std::lock_guard<std::mutex> lock(pendingMutex_);
        auto pit = pendingKtx_.find(path);
        if (pit != pendingKtx_.end()) {
            ktxTex = pit->second;
            pendingKtx_.erase(pit);
        }
    }

    // Fallback: load + transcode synchronously (no pre-load available).
    if (!ktxTex) {
        KTX_error_code result = ktxTexture_CreateFromNamedFile(
            path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex);
        if (result != KTX_SUCCESS) {
            LOG_E("KTX load failed: {} — {}", path, ktxErrorString(result));
            return 0;
        }
        if (ktxTex->classId == ktxTexture2_c) {
            ktxTexture2* ktxTex2 = reinterpret_cast<ktxTexture2*>(ktxTex);
            if (ktxTexture2_NeedsTranscoding(ktxTex2)) {
                result = ktxTexture2_TranscodeBasis(ktxTex2, KTX_TTF_BC7_RGBA, 0);
                if (result != KTX_SUCCESS) {
                    result = ktxTexture2_TranscodeBasis(ktxTex2, KTX_TTF_RGBA32, 0);
                }
                if (result != KTX_SUCCESS) {
                    LOG_E("KTX transcode failed: {} — {}", path, ktxErrorString(result));
                    ktxTexture_Destroy(ktxTex);
                    return 0;
                }
            }
        }
    }

    GLuint texID = 0;
    GLenum target = 0;
    GLenum glerr = GL_NO_ERROR;
    KTX_error_code result = ktxTexture_GLUpload(ktxTex, &texID, &target, &glerr);
    ktxTexture_Destroy(ktxTex);

    if (result != KTX_SUCCESS || glerr != GL_NO_ERROR) {
        LOG_E("KTX GL upload failed: {} result={} glerr={:#x}", path, ktxErrorString(result), glerr);
        return 0;
    }

    glBindTexture(target, texID);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(target, 0);

    LOG_I("KTX texture loaded: {} id={}", path, texID);
    textures_[path] = texID;
    return texID;
}
