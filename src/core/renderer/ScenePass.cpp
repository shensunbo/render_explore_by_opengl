#include "ScenePass.h"
#include "rhi/Rhi.h"

void ScenePass::execute(const FrameParams& params) {
    if (!shader_ || !meshes_) return;
    rhi::enableDepthTest(true);
    rhi::setClearColor(0.2f, 0.5f, 0.1f, 1.0f);
    rhi::clearColorDepth();

    shader_->use();
    if (skybox_) skybox_->ActiveCubeMap();

    const glm::mat4 mvp = params.projection * params.view * params.model;
    shader_->setMat4("model", params.model);
    shader_->setMat4("uMVP", mvp);
    shader_->setVec3("viewPosition", params.eye);
    shader_->setInt("cubemap", skybox_ ? skybox_->GetBindingPoint() : 0);

    for (const auto& v : *meshes_) {
        for (size_t i = 0; i < v.textures.size(); ++i) {
            auto uniform = v.textures[i].type;
            rhi::setActiveTexture(v.textures[i].bindId);
            shader_->setInt(uniform, v.textures[i].bindId);
            rhi::bindTexture2D(v.textures[i].id);
        }
        shader_->setBool("textureLoad", !v.textures.empty());
        v.bindVao();
        v.bindUbo();
        rhi::drawIndexedTriangles(v.getIndicesSize());
    }
}
