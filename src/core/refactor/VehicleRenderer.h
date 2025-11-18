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

public:
    VehicleShader* ourShader;
    VehicleMeshInfo* ourModel;
    // Skybox* cubemap;
    std::shared_ptr<Skybox> cubemap;
    std::shared_ptr<FboHandler> fbo_;
    ConfigParser  cfgParser;
};