#include "ScenePass.h"
#include "rhi/Rhi.h"

void ScenePass::execute(const FrameParams& params) {
    if (!shader_ || !meshes_) return;
    rhi::enableDepthTest(true);
    rhi::setClearColor(0.2f, 0.5f, 0.1f, 1.0f);
    rhi::clearColorDepth();

    shader_->use();
    if (skybox_) skybox_->ActiveCubeMap();

    // Build the effective instance list: use instanceModels if provided, else fall back to params.model.
    const std::vector<glm::mat4>* instanceMatrices = nullptr;
    std::vector<glm::mat4> fallback;
    if (!params.instanceModels.empty()) {
        instanceMatrices = &params.instanceModels;
    } else {
        fallback = {params.model};
        instanceMatrices = &fallback;
    }
    const int instanceCount = static_cast<int>(instanceMatrices->size());

    const glm::mat4 vp = params.projection * params.view;
    shader_->set("uVP", vp);
    shader_->set("viewPosition", params.eye);
    shader_->set("cubemap", skybox_ ? skybox_->GetBindingPoint() : 0u);

    for (const auto& v : *meshes_) {
        for (size_t i = 0; i < v.textures.size(); ++i) {
            auto uniform = v.textures[i].type;
            rhi::setActiveTexture(v.textures[i].bindId);
            shader_->set(uniform, v.textures[i].bindId);
            rhi::bindTexture2D(v.textures[i].id);
        }
        shader_->set("textureLoad", !v.textures.empty());
        v.updateInstanceBuffer(*instanceMatrices);
        v.bindVao();
        v.bindUbo();
        rhi::drawIndexedTrianglesInstanced(v.getIndicesSize(), instanceCount);
    }
}
