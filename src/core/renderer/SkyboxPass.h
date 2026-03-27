#pragma once

#include "IRenderPass.h"
#include "scene/Skybox.h"

/**
 * @class SkyboxPass
 * @brief Render pass that draws the cubemap skybox.
 *
 * Holds a non-owning pointer to the Skybox; lifetime of the Skybox must
 * exceed the lifetime of this pass.
 */
class SkyboxPass : public IRenderPass {
public:
    /**
     * @brief Constructs a SkyboxPass.
     * @param skybox Non-owning pointer to the skybox to render.
     */
    explicit SkyboxPass(Skybox* skybox) : skybox_(skybox) {}

    ~SkyboxPass() override = default;

    SkyboxPass(const SkyboxPass&)            = delete;
    SkyboxPass& operator=(const SkyboxPass&) = delete;

    SkyboxPass(SkyboxPass&&)            = default;
    SkyboxPass& operator=(SkyboxPass&&) = default;

    void execute(const FrameParams& params) override;

private:
    Skybox* skybox_; ///< Skybox to render (non-owning).
};
