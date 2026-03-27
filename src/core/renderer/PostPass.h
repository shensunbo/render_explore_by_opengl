#pragma once

#include "IRenderPass.h"
#include "postprocess/FboHandler.h"

/**
 * @class PostPass
 * @brief Render pass that blits the offscreen FBO to the fullscreen quad.
 *
 * Holds a non-owning pointer to the FboHandler; lifetime of the FboHandler
 * must exceed the lifetime of this pass.
 */
class PostPass : public IRenderPass {
public:
    /**
     * @brief Constructs a PostPass.
     * @param fbo Non-owning pointer to the framebuffer handler.
     */
    explicit PostPass(FboHandler* fbo) : fbo_(fbo) {}

    ~PostPass() override = default;

    PostPass(const PostPass&)            = delete;
    PostPass& operator=(const PostPass&) = delete;

    PostPass(PostPass&&)            = default;
    PostPass& operator=(PostPass&&) = default;

    void execute(const FrameParams& params) override;

private:
    FboHandler* fbo_; ///< Offscreen framebuffer handler (non-owning).
};
