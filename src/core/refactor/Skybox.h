#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "VehicleShader.h"

// TODO: 纹理ID先使用10，以避免和其他纹理冲突
class Skybox {
public:
    Skybox() = delete;
    // need assign bind point, cannot be repeated with existing ones
    Skybox(unsigned int bindPoint):shader_("res/shader/skybox.vs", "res/shader/skybox.fs"){
        bind_point_ = bindPoint;
    }
    ~Skybox() = default;

    bool Init(const std::vector<std::string>& faces);

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
    unsigned int LoadCubemap(const std::vector<std::string>& faces) const;

    unsigned int cubemap_;
    unsigned int bind_point_;

    GLuint vao_; 
    GLuint vbo_;

    VehicleShader shader_;
};