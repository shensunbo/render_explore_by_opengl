#pragma once

#include <memory>
#include <vector>
#include "VehicleShader.h"
#include "Skybox.h"
#include "FboHandler.h"
#include "CommonDataStruct.h"
#include "BufferObjectData.h"
#include <glm/glm.hpp>

struct FrameParams;

class IRenderPass {
public:
    virtual ~IRenderPass() = default;
    virtual void execute(const FrameParams& params) = 0;
};

class ScenePass : public IRenderPass {
public:
    ScenePass(VehicleShader* shader, Skybox* skybox,
              const std::vector<BufferObjectData>* meshes)
        : shader_(shader), skybox_(skybox), meshes_(meshes) {}
    void execute(const FrameParams& params) override;
private:
    VehicleShader* shader_;
    Skybox* skybox_;
    const std::vector<BufferObjectData>* meshes_;
};

class SkyboxPass : public IRenderPass {
public:
    SkyboxPass(Skybox* skybox) : skybox_(skybox) {}
    void execute(const FrameParams& params) override;
private:
    Skybox* skybox_;
};

class PostPass : public IRenderPass {
public:
    PostPass(FboHandler* fbo) : fbo_(fbo) {}
    void execute(const FrameParams& params) override;
private:
    FboHandler* fbo_;
};
