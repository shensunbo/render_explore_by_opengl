#pragma once

#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include "RenderPass.h"

struct PassTiming {
    std::string name;
    double milliseconds{0.0};
};

// A minimal render-graph style sequencer: owns a list of passes and executes
// them in order. It keeps existing pass interfaces intact to minimize churn.
class RenderGraph {
public:
    void addPass(const std::string& name, std::unique_ptr<IRenderPass> pass) {
        names_.emplace_back(name);
        passes_.emplace_back(std::move(pass));
    }

    void clear() {
        names_.clear();
        passes_.clear();
    }

    void execute(const FrameParams& params, std::vector<PassTiming>* timings = nullptr) {
        if (!timings) {
            for (auto& p : passes_) {
                p->execute(params);
            }
            return;
        }

        timings->clear();
        timings->reserve(passes_.size());
        for (size_t i = 0; i < passes_.size(); ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            passes_[i]->execute(params);
            auto end = std::chrono::high_resolution_clock::now();
            double ms = std::chrono::duration<double, std::milli>(end - start).count();
            timings->push_back(PassTiming{names_[i], ms});
        }
    }

private:
    std::vector<std::string> names_;
    std::vector<std::unique_ptr<IRenderPass>> passes_;
};
