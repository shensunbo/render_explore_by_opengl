#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"

// TODO: 纹理ID先使用10，以避免和其他纹理冲突
class Skybox {
public:
    Skybox() = delete;
    // need assign bind point, cannot be repeated with existing ones
    Skybox(unsigned int bindPoint){
        bind_point_ = bindPoint;
    }
    ~Skybox() = default;

    bool Init(const std::vector<std::string>& faces);

    void ActiveCubeMap() const;

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
};