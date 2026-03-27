#pragma once 
#include "Shader.h"
#include "Skybox.h"
#include "MeshInfo.h"
#include "configParser/ConfigParser.h"
#include "FboHandler.h"
#include "TextureCache.h"
#include "RenderPass.h"
#include "RenderGraph.h"

#include <array>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct RendererConfig {
    unsigned int width{0};
    unsigned int height{0};
    // resource root path, e.g. "" for run-from-root or "../" for out-of-tree
    std::string resourceRoot;
    std::string modelPath;
    std::string infoPath;
    std::string vsPath;
    std::string fsPath;
    std::string pbrFsPath;
    std::array<std::string, 6> skyboxFaces{};
    bool enableFbo{true};
};

struct FrameParams {
    glm::mat4 projection{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 model{1.0f};
    glm::vec3 eye{0.0f, 0.0f, 0.0f};
    bool enableFbo{false};
    bool dumpOnce{false};
};

class Renderer {
public:
    void create(const RendererConfig& cfg);
    void destroy();
    void update();
    void draw();
    void renderFrame(const FrameParams& params);
    void resize(unsigned int width, unsigned int height);
    void setTimingEnabled(bool enabled) { timingEnabled_ = enabled; }
    void setPbrEnabled(bool enabled);
    bool isPbrEnabled() const { return usePbr_; }
    Shader* activeShader() const { return activeShader_; }
    void setCubemapRotation(const glm::mat4& rotation);

private:
    void releaseTextureData();
    void cleanupGpuTextures();
    void rebuildGraphs();
    void ensureFbo();
    void rebuildFbo(unsigned int width, unsigned int height);
    void applyCubemapRotation();

public:
    std::unique_ptr<Shader> legacyShader_;
    std::unique_ptr<Shader> pbrShader_;
    Shader* activeShader_{nullptr};
    std::unique_ptr<MeshInfo> ourModel;
    // Skybox* cubemap;
    std::shared_ptr<Skybox> cubemap;
    std::shared_ptr<FboHandler> fbo_;
    ConfigParser  cfgParser;
    std::unique_ptr<TextureCache> textureCache_;

    std::unique_ptr<RenderGraph> onscreenGraph_;
    std::unique_ptr<RenderGraph> fboGraph_;

    bool timingEnabled_{false};
    std::vector<PassTiming> timings_;

    // cached config needed for lazy FBO creation / graph rebuild
    unsigned int width_{0};
    unsigned int height_{0};
    std::string resRoot_;
    std::string fboVsPath_;
    std::string fboFsPath_;
    std::vector<std::string> skyboxFaces_;
    std::string modelPath_;
    glm::mat4 cubemapRotation_{1.0f};
    bool usePbr_{false};

    std::set<std::string> m_texture_paths;
    std::unordered_map<std::string, imageParam> m_loaded_texture_data;
};