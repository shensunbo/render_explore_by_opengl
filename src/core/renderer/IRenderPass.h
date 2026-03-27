#pragma once

#include "FrameParams.h"

/**
 * @class IRenderPass
 * @brief Abstract interface for a single render pass.
 *
 * Derived classes implement execute() to perform their specific rendering work.
 * Copy is deleted to prevent object slicing through the base pointer; move is
 * allowed so passes can be transferred into RenderGraph.
 */
class IRenderPass {
public:
    IRenderPass() = default;
    virtual ~IRenderPass() = default;

    IRenderPass(const IRenderPass&)            = delete;
    IRenderPass& operator=(const IRenderPass&) = delete;
    IRenderPass(IRenderPass&&)                 = default;
    IRenderPass& operator=(IRenderPass&&)      = default;

    /**
     * @brief Execute this pass using the supplied per-frame parameters.
     * @param params Frame matrices, eye position, and control flags.
     */
    virtual void execute(const FrameParams& params) = 0;
};
