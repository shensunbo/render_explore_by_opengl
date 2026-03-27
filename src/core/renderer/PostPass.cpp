#include "PostPass.h"

void PostPass::execute(const FrameParams& params) {
    if (!fbo_) return;
    if (params.dumpOnce) {
        fbo_->dumpTextureToFile("dump.png");
    }
    fbo_->renderToFullscreenQuad();
}
