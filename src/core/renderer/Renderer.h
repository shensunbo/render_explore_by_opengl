#pragma once
#include "FrameParams.h"

#include <array>
#include <memory>
#include <string>
#include <glm/glm.hpp>

class Shader;

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

class Renderer {
public:
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void create(const RendererConfig& cfg);
    void destroy();
    void update();
    void draw();

    void renderFrame(const FrameParams& params);
    void resize(unsigned int width, unsigned int height);
    void setTimingEnabled(bool enabled);
    void setPbrEnabled(bool enabled);
    bool isPbrEnabled() const;
    Shader* activeShader() const;
    void setCubemapRotation(const glm::mat4& rotation);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};