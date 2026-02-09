#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include "VehicleShader.h"
#include "CommonDataStruct.h"

// TODO: 纹理ID先使用10，以避免和其他纹理冲突
class Skybox {
public:
    Skybox() = delete;
    // need assign bind point, cannot be repeated with existing ones
    Skybox(unsigned int bindPoint, std::string vs = "res/shader/skybox.vs", std::string fs = "res/shader/skybox.fs"):shader_(vs.c_str(), fs.c_str()){
        bind_point_ = bindPoint;
    }
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;
    Skybox(Skybox&&) = delete;
    Skybox& operator=(Skybox&&) = delete;

    bool Init(const std::vector<std::string>& faces, const std::unordered_map<std::string, imageParam>& textureData);

    void ActiveCubeMap() const;

    // display skybox
    void initSkybox();
    void drawSkybox();
    void updateMvpMatrix(const glm::mat4& mvp) {
        shader_.use();
        shader_.setMat4("uMVP", mvp);
    }


    unsigned int GetCubemap() const {
        return cubemap_;
    }

    unsigned int GetBindingPoint() const {
        return bind_point_;
    }

private:
    // load from disk, slow
    unsigned int LoadCubemap(const std::vector<std::string>& faces) const;
    // load from buffer
    unsigned int LoadCubemap(const std::vector<std::string>& faces, const std::unordered_map<std::string, imageParam>& textureData) const;

    unsigned int cubemap_{0};
    unsigned int bind_point_{0};

    GLuint vao_{0}; 
    GLuint vbo_{0};

    VehicleShader shader_;
};