#pragma once 
#include "VehicleShader.h"
#include "Skybox.h"
#include "VehicleMeshInfo.h"
#include "configParser/ConfigParser.h"
#include "FboHandler.h"

#include <array>
#include <memory>

struct RendererConfig {
    unsigned int width{0};
    unsigned int height{0};
    // resource root path, e.g. "" for run-from-root or "../" for out-of-tree
    std::string resourceRoot;
    std::string modelPath;
    std::string vehicleInfoPath;
    std::string vehicleVsPath;
    std::string vehicleFsPath;
    std::array<std::string, 6> skyboxFaces{};
    bool enableFbo{true};
};

class VehicleRenderer {
public:
    void create(const RendererConfig& cfg);
    void destroy();
    void update();
    void draw();

private:
    void releaseTextureData();

public:
    std::unique_ptr<VehicleShader> ourShader;
    std::unique_ptr<VehicleMeshInfo> ourModel;
    // Skybox* cubemap;
    std::shared_ptr<Skybox> cubemap;
    std::shared_ptr<FboHandler> fbo_;
    ConfigParser  cfgParser;

    std::set<std::string> m_texture_paths;
    std::unordered_map<std::string, imageParam> m_loaded_texture_data;
};