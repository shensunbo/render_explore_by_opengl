#include "RenderPass.h"
#include "VehicleRenderer.h" // for FrameParams

void ScenePass::execute(const FrameParams& params) {
    if (!shader_ || !meshes_) return;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.5f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_->use();
    if (skybox_) skybox_->ActiveCubeMap();

    const glm::mat4 mvp = params.projection * params.view * params.model;
    shader_->setMat4("uMVP", mvp);
    shader_->setVec3("viewPosition", params.eye);
    shader_->setInt("cubemap", skybox_ ? skybox_->GetBindingPoint() : 0);

    for (const auto& v : *meshes_) {
        for (size_t i = 0; i < v.textures.size(); ++i) {
            auto uniform = v.textures[i].type;
            glActiveTexture(GL_TEXTURE0 + v.textures[i].bindId);
            shader_->setInt(uniform, v.textures[i].bindId);
            glBindTexture(GL_TEXTURE_2D, v.textures[i].id);
        }
        shader_->setBool("textureLoad", !v.textures.empty());
        v.bindVao();
        v.bindUbo();
        glDrawElements(GL_TRIANGLES, v.getIndicesSize(), GL_UNSIGNED_INT, 0);
    }
}

void SkyboxPass::execute(const FrameParams& params) {
    if (!skybox_) return;
    glm::mat4 skyView = glm::mat4(glm::mat3(params.view));
    glm::mat4 skyMvp = params.projection * skyView;
    skybox_->updateMvpMatrix(skyMvp);
    skybox_->drawSkybox();
}

void PostPass::execute(const FrameParams& params) {
    if (!fbo_) return;
    if (params.dumpOnce) {
        fbo_->dumpTextureToFile("dump.png");
    }
    fbo_->renderToFullscreenQuad();
}
