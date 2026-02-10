#include "VehicleRenderer.h"
#include "log/mylog.h"
#include <memory>
#include <mutex>
#include <stb_image.h>
#include <thread>
#include <unordered_set>
#include "tool.h"
#include "RenderPass.h"
#include "RenderGraph.h"

void VehicleRenderer::create(const RendererConfig& cfg){

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
    const std::string cfgPath = cfg.vehicleInfoPath.empty()
                                    ? make_path(prefix, "res/model/halo/vehicle_info.json")
                                    : make_path(prefix, cfg.vehicleInfoPath);
    mylog(LogLevel::I, "Loading config file: %s", cfgPath.c_str());
    cfgParser.loadConfigFile(cfgPath, m_texture_paths);

    const std::string vs_path = cfg.vehicleVsPath.empty()
                                    ? make_path(prefix, "res/shader/with_texture.vs")
                                    : make_path(prefix, cfg.vehicleVsPath);
    const std::string fs_path = cfg.vehicleFsPath.empty()
                                    ? make_path(prefix, "res/shader/with_texture.fs")
                                    : make_path(prefix, cfg.vehicleFsPath);
    ourShader = std::make_unique<VehicleShader>(vs_path.c_str(), fs_path.c_str());

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
    ourModel = std::make_unique<VehicleMeshInfo>(path, cfgParser, m_loaded_texture_data, *textureCache_);

    for(auto& it : ourModel->meshes) {
        unsigned int blockIndex = ourShader->getBlockIndex("MaterialBlock");
        ourShader->uniformBlockBind(blockIndex, 0);
        it.updateUbo(it.mUboMat);
        mylog(LogLevel::D, "VehicleRenderer::create: mesh name: %s, MaterialName: %s", it.meshName.c_str(), it.mMaterial.MaterialName.c_str());
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
    mylog(LogLevel::I, "VehicleRenderer::create");
}

void VehicleRenderer::destroy(){
    mylog(LogLevel::I, "VehicleRenderer::destroy");
    cleanupGpuTextures();
    if (textureCache_) {
        textureCache_->destroy();
        textureCache_.reset();
    }
    ourShader.reset();
    ourModel.reset();
}

void VehicleRenderer::update(){
    mylog(LogLevel::I, "VehicleRenderer::update");
}

void VehicleRenderer::draw(){
    for(auto& v :  ourModel->meshes){
        for (size_t i = 0; i < v.textures.size(); ++i) {
            auto uniform = v.textures[i].type;
            glActiveTexture(GL_TEXTURE0 + v.textures[i].bindId);
            ourShader->setInt(uniform, v.textures[i].bindId);
            glBindTexture(GL_TEXTURE_2D, v.textures[i].id);

            // ourShader->setBool((uniform + "_load"), true);
            // ourShader->setBool("textureLoad", true);
        }

        if(v.textures.size()){
            ourShader->setBool("textureLoad", true);
        }else{
            ourShader->setBool("textureLoad", false);
        }

    // Draw current mesh.
        v.bindVao();
        v.bindUbo();

        glDrawElements(GL_TRIANGLES, v.getIndicesSize(), GL_UNSIGNED_INT, 0);

        // ourShader->setBool("textureLoad", false);
    }

    // mylog(LogLevel::I, "VehicleRenderer::draw");
}

void VehicleRenderer::renderFrame(const FrameParams& params){
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
            fboGraph_->execute(params);
        }
    } else {
        if (onscreenGraph_) {
            onscreenGraph_->execute(params);
        }
    }
}

void VehicleRenderer::rebuildGraphs(){
    onscreenGraph_ = std::make_unique<RenderGraph>();
    onscreenGraph_->addPass(std::make_unique<ScenePass>(ourShader.get(), cubemap.get(), &ourModel->meshes));
    onscreenGraph_->addPass(std::make_unique<SkyboxPass>(cubemap.get()));

    fboGraph_ = std::make_unique<RenderGraph>();
    fboGraph_->addPass(std::make_unique<ScenePass>(ourShader.get(), cubemap.get(), &ourModel->meshes));
    fboGraph_->addPass(std::make_unique<SkyboxPass>(cubemap.get()));
    fboGraph_->addPass(std::make_unique<PostPass>(fbo_.get()));
}

void VehicleRenderer::ensureFbo(){
    if (fbo_) return;
    if (width_ == 0 || height_ == 0) return;
    fbo_ = std::make_shared<FboHandler>(width_, height_, fboVsPath_, fboFsPath_);
    fbo_->init();
}
void VehicleRenderer::cleanupGpuTextures(){
    if (!textureCache_) return;
    textureCache_->destroy();
}

void VehicleRenderer::releaseTextureData(){
    for (const auto& tex : m_loaded_texture_data) {
        if (tex.second.data) {
            stbi_image_free(tex.second.data);
        }
    }

    m_loaded_texture_data.clear();
    mylog(LogLevel::I, "All texture data released.");
}