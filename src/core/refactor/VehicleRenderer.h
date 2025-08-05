#pragma once 
#include "VehicleShader.h"
#include "Skybox.h"
#include "VehicleMeshInfo.h"
#include "configParser/ConfigParser.h"
#include "memory"

class VehicleRenderer {
public:
    void create();
    void destroy();
    void update();
    void draw();

public:
    VehicleShader* ourShader;
    VehicleMeshInfo* ourModel;
    // Skybox* cubemap;
    std::shared_ptr<Skybox> cubemap;
    ConfigParser  cfgParser;
};