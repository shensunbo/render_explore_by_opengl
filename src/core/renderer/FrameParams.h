#pragma once

#include <glm/glm.hpp>
#include <vector>

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
    /// Per-instance model matrices for instanced rendering.
    /// When non-empty, overrides the single params.model; each element is one instance.
    std::vector<glm::mat4> instanceModels;
};
