#pragma once

#include <glm/glm.hpp>
#include <string>
#include "core/refactor/VehicleRenderer.h"

class RendererAPI {
public:
    RendererAPI();
    ~RendererAPI();

    void init(int width, int height, const std::string& resourcePrefix = "");
    void deinit();
    void update();
    void render();

private:
    VehicleRenderer vRender;
    int width;
    int height;
    
    // Model matrices
    glm::mat4 model;
    glm::mat4 skyboxModel;
    glm::mat4 projection;
    glm::mat4 view;
};