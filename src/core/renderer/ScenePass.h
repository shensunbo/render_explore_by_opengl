#pragma once

#include "IRenderPass.h"
#include "shader/Shader.h"
#include "scene/Skybox.h"
#include "mesh/BufferObjectData.h"
#include <vector>

/**
 * @class ScenePass
 * @brief Render pass that draws all scene meshes with the active shader.
 *
 * Holds non-owning observer pointers; lifetime of shader, skybox, and
 * meshes must exceed the lifetime of this pass.
 */
class ScenePass : public IRenderPass {
public:
    /**
     * @brief Constructs a ScenePass.
     * @param shader  Non-owning pointer to the active shader program.
     * @param skybox  Non-owning pointer to the skybox (may be nullptr).
     * @param meshes  Non-owning pointer to the loaded mesh list.
     */
    ScenePass(Shader* shader, Skybox* skybox,
              const std::vector<BufferObjectData>* meshes)
        : shader_(shader), skybox_(skybox), meshes_(meshes) {}

    ~ScenePass() override = default;

    // Non-copyable: raw pointer members are observers, not owners, but
    // duplicating a pass would create ambiguous rendering responsibility.
    ScenePass(const ScenePass&)            = delete;
    ScenePass& operator=(const ScenePass&) = delete;

    // Movable: allows transfer into RenderGraph without allocation.
    ScenePass(ScenePass&&)            = default;
    ScenePass& operator=(ScenePass&&) = default;

    void execute(const FrameParams& params) override;

private:
    Shader*                              shader_; ///< Active shader program (non-owning).
    Skybox*                              skybox_; ///< Skybox for cubemap binding (non-owning).
    const std::vector<BufferObjectData>* meshes_; ///< Scene mesh list (non-owning).
};
