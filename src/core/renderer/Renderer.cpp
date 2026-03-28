#include "Renderer.h"
#include "shader/Shader.h"
#include "scene/Skybox.h"
#include "mesh/MeshInfo.h"
#include "configParser/ConfigParser.h"
#include "postprocess/FboHandler.h"
#include "texture/TextureCache.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "log/mylog.h"
#include "common/tool.h"

#include <memory>
#include <mutex>
#include <set>
#include <stb_image.h>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <glm/glm.hpp>

// ---------------------------------------------------------------------------
// Renderer::Impl – owns all private state and implements all logic
// ---------------------------------------------------------------------------
class Renderer::Impl {
public:
    // --- state (formerly Renderer private members) ---
    std::unique_ptr<Shader> legacyShader_;
    std::unique_ptr<Shader> pbrShader_;
    Shader* activeShader_{nullptr}; // non-owning pointer to currently active shader
    std::unique_ptr<MeshInfo> ourModel;
    std::shared_ptr<Skybox> cubemap;
    std::shared_ptr<FboHandler> fbo_;
    ConfigParser cfgParser;
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

    // --- forwarded public operations ---
    void create(const RendererConfig& cfg);
    void destroy();
    void update();
    void draw();
    void renderFrame(const FrameParams& params);
    void resize(unsigned int width, unsigned int height);
    void setPbrEnabled(bool enabled);
    void setCubemapRotation(const glm::mat4& rotation);

private:
    // --- private helpers (formerly Renderer private methods) ---
    void releaseTextureData();
    void cleanupGpuTextures();
    void rebuildGraphs();
    void ensureFbo();
    void rebuildFbo(unsigned int width, unsigned int height);
    void applyCubemapRotation();
};

// ---------------------------------------------------------------------------
// Renderer::Impl method implementations
// ---------------------------------------------------------------------------

void Renderer::Impl::create(const RendererConfig& cfg){

    // Helper to prefix resource paths with an optional root (works for in-tree and out-of-tree runs).
    auto make_path = [](const std::string& root, const std::string& relative) {
        if (relative.empty()) return std::string();
        if (root.empty()) return relative;
        if (root.back() == '/') return root + relative;
        return root + "/" + relative;
    };

    const std::string prefix = cfg.resourceRoot;
    width_ = cfg.width;
    height_ = cfg.height;
    resRoot_ = cfg.resourceRoot;

    // Resolve config and shader paths with reasonable defaults.
    const std::string cfgPath = cfg.infoPath.empty()
                                    ? make_path(prefix, "res/model/halo/vehicle_info.json")
                                    : make_path(prefix, cfg.infoPath);
    LOG_I("Loading config file: {}", cfgPath);
    cfgParser.loadConfigFile(cfgPath, m_texture_paths);

    const std::string vs_path = cfg.vsPath.empty()
                                    ? make_path(prefix, "res/shader/with_texture.vs")
                                    : make_path(prefix, cfg.vsPath);
    const std::string fs_path = cfg.fsPath.empty()
                                    ? make_path(prefix, "res/shader/with_texture.fs")
                                    : make_path(prefix, cfg.fsPath);
    const std::string pbr_fs_path = cfg.pbrFsPath.empty()
                                        ? make_path(prefix, "res/shader/pbr.fs")
                                        : make_path(prefix, cfg.pbrFsPath);
    legacyShader_ = std::make_unique<Shader>(vs_path.c_str(), fs_path.c_str());
    pbrShader_ = std::make_unique<Shader>(vs_path.c_str(), pbr_fs_path.c_str());
    activeShader_ = legacyShader_.get();

    textureCache_ = std::make_unique<TextureCache>();
    
    const std::string path = cfg.modelPath.empty()
                                 ? make_path(prefix, "res/model/halo/halo.fbx")
                                 : make_path(prefix, cfg.modelPath);
    modelPath_ = path;

    // Build skybox face list.
    std::vector<std::string> faces;
    if (!cfg.skyboxFaces[0].empty()) {
        faces.assign(cfg.skyboxFaces.begin(), cfg.skyboxFaces.end());
        for (auto& f : faces) {
            f = make_path(prefix, f);
        }
    } else {
        faces = {
            make_path(prefix, "res/model/skybox/px.png"),
            make_path(prefix, "res/model/skybox/nx.png"),
            make_path(prefix, "res/model/skybox/py.png"),
            make_path(prefix, "res/model/skybox/ny.png"),
            make_path(prefix, "res/model/skybox/pz.png"),
            make_path(prefix, "res/model/skybox/nz.png"),
        };
    }

    skyboxFaces_ = faces;

    m_texture_paths.insert(faces[0]);
    m_texture_paths.insert(faces[1]);
    m_texture_paths.insert(faces[2]);
    m_texture_paths.insert(faces[3]);
    m_texture_paths.insert(faces[4]);
    m_texture_paths.insert(faces[5]);

    // Multithreaded texture loading: gather raw pixel data up-front, then upload on the main thread.
    std::mutex textureMutex;
    std::vector<std::thread> threads;
    threads.reserve(m_texture_paths.size());
    for(auto const& texture_path : m_texture_paths) {
        threads.emplace_back([this, &textureMutex, texture_path]() {
            imageParam img{};
            auto result = Tool::ImageFromFile(texture_path, img);
            if (result == 0) {
                std::lock_guard<std::mutex> lock(textureMutex);
                m_loaded_texture_data[texture_path] = img;
            }
        });
    }

    // Wait for all texture-loading threads to complete.
    for (auto& thread : threads) {
        thread.join();
    }

    // Build GPU meshes using the loaded texture data and cache.
    ourModel = std::make_unique<MeshInfo>(path, cfgParser, m_loaded_texture_data, *textureCache_);

    for(auto& it : ourModel->meshes_) {
        if (legacyShader_) {
            unsigned int blockIndex = legacyShader_->getBlockIndex("MaterialBlock");
            legacyShader_->uniformBlockBind(blockIndex, 0);
        }
        if (pbrShader_) {
            unsigned int blockIndex = pbrShader_->getBlockIndex("MaterialBlock");
            pbrShader_->uniformBlockBind(blockIndex, 0);
        }
        it.updateUbo(it.mUboMat);
        LOG_D("Renderer::create: mesh name: {}, MaterialName: {}", it.meshName, it.mMaterial.MaterialName);
    }

    unsigned int skyboxBindID = ourModel->getMaxTextureID() + 1;
    cubemap = std::make_shared<Skybox>(skyboxBindID, make_path(prefix, "res/shader/skybox.vs"), make_path(prefix, "res/shader/skybox.fs"));
    cubemap->Init(faces, m_loaded_texture_data);

    // Optional FBO for post-processing or dumps.
    fboVsPath_ = make_path(prefix, "res/shader/fbo_rect.vs");
    fboFsPath_ = make_path(prefix, "res/shader/fbo_rect.fs");
    if (cfg.enableFbo) {
        fbo_ = std::make_shared<FboHandler>(cfg.width, cfg.height, fboVsPath_, fboFsPath_);
        fbo_->init();
    } else {
        fbo_.reset();
    }

    // Build render graphs: onscreen (scene + skybox) and fbo (scene + skybox + post).
    rebuildGraphs();

    releaseTextureData();
    applyCubemapRotation();
    LOG_I("Renderer::create");
}

void Renderer::Impl::destroy(){
    LOG_I("Renderer::destroy");
    cleanupGpuTextures();
    if (textureCache_) {
        textureCache_->destroy();
        textureCache_.reset();
    }
    legacyShader_.reset();
    pbrShader_.reset();
    activeShader_ = nullptr;
    ourModel.reset();
    onscreenGraph_.reset();
    fboGraph_.reset();
    fbo_.reset();
}

void Renderer::Impl::update(){
    LOG_I("Renderer::update");
}

void Renderer::Impl::draw(){
    if (!activeShader_) return;
    for(auto& v :  ourModel->meshes_){
        for (size_t i = 0; i < v.textures.size(); ++i) {
            auto uniform = v.textures[i].type;
            glActiveTexture(GL_TEXTURE0 + v.textures[i].bindId);
            activeShader_->set(uniform, v.textures[i].bindId);
            glBindTexture(GL_TEXTURE_2D, v.textures[i].id);

            // activeShader_->set((uniform + "_load"), true);
            // activeShader_->set("textureLoad", true);
        }

        if(v.textures.size()){
            activeShader_->set("textureLoad", true);
        }else{
            activeShader_->set("textureLoad", false);
        }

    // Draw current mesh.
        v.bindVao();
        v.bindUbo();

        glDrawElements(GL_TRIANGLES, v.getIndicesSize(), GL_UNSIGNED_INT, 0);

        // activeShader_->setBool("textureLoad", false);
    }

    // LOG_I("Renderer::draw");
}

void Renderer::Impl::renderFrame(const FrameParams& params){
    // Choose between onscreen render and offscreen FBO path.
    const bool wantFbo = params.enableFbo;
    const bool hasFbo = fbo_ != nullptr;

    if (wantFbo && !hasFbo) {
        ensureFbo();
        rebuildGraphs();
    }

    const bool useFbo = wantFbo && fbo_ != nullptr;
    if (useFbo) {
        fbo_->enable();
        if (fboGraph_) {
            if (timingEnabled_) {
                fboGraph_->execute(params, &timings_);
            } else {
                fboGraph_->execute(params);
            }
        }
    } else {
        if (onscreenGraph_) {
            if (timingEnabled_) {
                onscreenGraph_->execute(params, &timings_);
            } else {
                onscreenGraph_->execute(params);
            }
        }
    }

    if (timingEnabled_ && !timings_.empty()) {
        double totalMs = 0.0;
        for (const auto& t : timings_) totalMs += t.milliseconds;
        LOG_I("Frame timings: total={:.3f} ms", totalMs);
        for (const auto& t : timings_) {
            LOG_I("  {}: {:.3f} ms", t.name, t.milliseconds);
        }
    }
}

void Renderer::Impl::resize(unsigned int width, unsigned int height){
    width_ = width;
    height_ = height;
    if (width_ == 0 || height_ == 0) return;

    // Recreate FBO if it exists or if future frames enable it.
    rebuildFbo(width_, height_);
    rebuildGraphs();
}

void Renderer::Impl::rebuildGraphs(){
    onscreenGraph_ = std::make_unique<RenderGraph>();
    onscreenGraph_->addPass("ScenePass", std::make_unique<ScenePass>(activeShader_, cubemap.get(), &ourModel->meshes_));
    onscreenGraph_->addPass("SkyboxPass", std::make_unique<SkyboxPass>(cubemap.get()));

    fboGraph_ = std::make_unique<RenderGraph>();
    fboGraph_->addPass("ScenePass", std::make_unique<ScenePass>(activeShader_, cubemap.get(), &ourModel->meshes_));
    fboGraph_->addPass("SkyboxPass", std::make_unique<SkyboxPass>(cubemap.get()));
    fboGraph_->addPass("PostPass", std::make_unique<PostPass>(fbo_.get()));
}

void Renderer::Impl::ensureFbo(){
    if (fbo_) return;
    if (width_ == 0 || height_ == 0) return;
    fbo_ = std::make_shared<FboHandler>(width_, height_, fboVsPath_, fboFsPath_);
    fbo_->init();
}

void Renderer::Impl::rebuildFbo(unsigned int width, unsigned int height){
    // If FBO is not desired, just drop it; it will be recreated lazily when needed.
    if (!fbo_) {
        return;
    }
    fbo_.reset();
    fbo_ = std::make_shared<FboHandler>(width, height, fboVsPath_, fboFsPath_);
    fbo_->init();
}

void Renderer::Impl::cleanupGpuTextures(){
    if (!textureCache_) return;
    textureCache_->destroy();
}

void Renderer::Impl::setPbrEnabled(bool enabled) {
    bool wantPbr = enabled && pbrShader_ != nullptr;
    Shader* newShader = wantPbr ? pbrShader_.get() : legacyShader_.get();
    if (activeShader_ == newShader) {
        usePbr_ = wantPbr;
        return;
    }
    usePbr_ = wantPbr;
    activeShader_ = newShader;
    rebuildGraphs();
    applyCubemapRotation();
}

void Renderer::Impl::setCubemapRotation(const glm::mat4& rotation) {
    cubemapRotation_ = rotation;
    applyCubemapRotation();
}

void Renderer::Impl::applyCubemapRotation() {
    if (activeShader_) {
        activeShader_->use();
        activeShader_->set("cubemapRotateMatrix", cubemapRotation_);
    }
    if (legacyShader_ && activeShader_ != legacyShader_.get()) {
        legacyShader_->use();
        legacyShader_->set("cubemapRotateMatrix", cubemapRotation_);
    }
    if (pbrShader_ && activeShader_ != pbrShader_.get()) {
        pbrShader_->use();
        pbrShader_->set("cubemapRotateMatrix", cubemapRotation_);
    }
}

void Renderer::Impl::releaseTextureData(){
    for (const auto& tex : m_loaded_texture_data) {
        if (tex.second.data) {
            stbi_image_free(tex.second.data);
        }
    }

    m_loaded_texture_data.clear();
    LOG_I("All texture data released.");
}

// ---------------------------------------------------------------------------
// Renderer – thin delegation wrappers
// ---------------------------------------------------------------------------

Renderer::Renderer() : pImpl_(std::make_unique<Impl>()) {}
Renderer::~Renderer() = default;

void Renderer::create(const RendererConfig& cfg)              { pImpl_->create(cfg); }
void Renderer::destroy()                                      { pImpl_->destroy(); }
void Renderer::update()                                       { pImpl_->update(); }
void Renderer::draw()                                         { pImpl_->draw(); }
void Renderer::renderFrame(const FrameParams& params)         { pImpl_->renderFrame(params); }
void Renderer::resize(unsigned int w, unsigned int h)         { pImpl_->resize(w, h); }
void Renderer::setTimingEnabled(bool enabled)                 { pImpl_->timingEnabled_ = enabled; }
void Renderer::setPbrEnabled(bool enabled)                    { pImpl_->setPbrEnabled(enabled); }
bool Renderer::isPbrEnabled() const                           { return pImpl_->usePbr_; }
Shader* Renderer::activeShader() const                        { return pImpl_->activeShader_; }
void Renderer::setCubemapRotation(const glm::mat4& rotation)  { pImpl_->setCubemapRotation(rotation); }