#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include "shader/Shader.h"
#include "common/CommonDataStruct.h"
#include <ktx.h>

// TODO: Use texture binding IDs starting at 10 to avoid conflicts with other textures.
class Skybox {
public:
    Skybox() = delete;
    // Assign a unique bind point that does not collide with existing textures.
    Skybox(unsigned int bindPoint, std::string vs = "res/shader/skybox.vs", std::string fs = "res/shader/skybox.fs"):shader_(vs.c_str(), fs.c_str()){
        bind_point_ = bindPoint;
    }
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;
    Skybox(Skybox&&) = delete;
    Skybox& operator=(Skybox&&) = delete;

    /**
     * @brief Initialise the skybox from PNG faces pre-loaded into textureData.
     * If any face path ends in .ktx or .ktx2, the KTX path is used automatically.
     */
    bool Init(const std::vector<std::string>& faces, const std::unordered_map<std::string, imageParam>& textureData);

    void ActiveCubeMap() const;

    // Render the skybox.
    void initSkybox();
    void drawSkybox();
    void updateMvpMatrix(const glm::mat4& mvp) {
        shader_.use();
        shader_.set("uMVP", mvp);
    }


    unsigned int GetCubemap() const {
        return cubemap_;
    }

    unsigned int GetBindingPoint() const {
        return bind_point_;
    }

private:
    // Load skybox textures from disk (slow path).
    unsigned int LoadCubemap(const std::vector<std::string>& faces) const;
    // Load skybox textures from preloaded stb buffers.
    unsigned int LoadCubemap(const std::vector<std::string>& faces, const std::unordered_map<std::string, imageParam>& textureData) const;
    // Load skybox textures from KTX2 files (CPU transcode → RGBA32 → GL upload per face).
    unsigned int LoadCubemapKtx(const std::vector<std::string>& faces) const;

    unsigned int cubemap_{0};
    unsigned int bind_point_{0};

    GLuint vao_{0}; 
    GLuint vbo_{0};

    Shader shader_;
};