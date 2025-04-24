#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"

class Skybox {
public:
    Skybox() = default;
    ~Skybox() = default;

    bool Init(const std::vector<std::string>& faces);

    void ActiveCubeMap() const;

    unsigned int GetCubemap() const {
        return cubemap_;
    }

private:
    unsigned int LoadCubemap(const std::vector<std::string>& faces) const;

    unsigned int cubemap_;
};