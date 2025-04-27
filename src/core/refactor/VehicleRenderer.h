#pragma once 
#include "VehicleShader.h"
#include "Skybox.h"
#include "VehicleMeshInfo.h"
class VehicleRenderer {
public:
    void create();
    void destroy();
    void update();
    void draw();

public:
    VehicleShader* ourShader;
    VehicleMeshInfo* ourModel;
    Skybox cubemap;
};