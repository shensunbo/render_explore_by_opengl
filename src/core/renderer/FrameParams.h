#pragma once

#include <glm/glm.hpp>

/**
 * @struct FrameParams
 * @brief Parameters passed to each render pass for a single frame.
 */
struct FrameParams {
    glm::mat4 projection{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 model{1.0f};
    glm::vec3 eye{0.0f, 0.0f, 0.0f};
    bool enableFbo{false};
    bool dumpOnce{false};
};
