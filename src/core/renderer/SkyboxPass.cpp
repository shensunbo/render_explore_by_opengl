#include "SkyboxPass.h"

void SkyboxPass::execute(const FrameParams& params) {
    if (!skybox_) return;
    glm::mat4 skyView = glm::mat4(glm::mat3(params.view));
    glm::mat4 skyMvp  = params.projection * skyView;
    skybox_->updateMvpMatrix(skyMvp);
    skybox_->drawSkybox();
}
