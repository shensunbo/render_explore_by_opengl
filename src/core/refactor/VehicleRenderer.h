#pragma once 
#include "VehicleShader.h"
#include "Skybox.h"
#include "VehicleMeshInfo.h"
#include "configParser/ConfigParser.h"
#include "FboHandler.h"

#include <memory>

class VehicleRenderer {
public:
    void create(unsigned int width, unsigned int height, const std::string& resourcePrefix = "");
    void destroy();
    void update();
    void draw();

private:
    void releaseTextureData();

public:
    VehicleShader* ourShader;
    VehicleMeshInfo* ourModel;
    // Skybox* cubemap;
    std::shared_ptr<Skybox> cubemap;
    std::shared_ptr<FboHandler> fbo_;
    ConfigParser  cfgParser;

    std::set<std::string> m_texture_paths;
    std::unordered_map<std::string, imageParam> m_loaded_texture_data;
};