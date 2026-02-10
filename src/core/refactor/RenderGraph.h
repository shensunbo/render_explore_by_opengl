#pragma once

#include <memory>
#include <vector>
#include "RenderPass.h"

// A minimal render-graph style sequencer: owns a list of passes and executes
// them in order. It keeps existing pass interfaces intact to minimize churn.
class RenderGraph {
public:
    void addPass(std::unique_ptr<IRenderPass> pass) {
        passes_.emplace_back(std::move(pass));
    }

    void clear() { passes_.clear(); }

    void execute(const FrameParams& params) {
        for (auto& p : passes_) {
            p->execute(params);
        }
    }

private:
    std::vector<std::unique_ptr<IRenderPass>> passes_;
};
