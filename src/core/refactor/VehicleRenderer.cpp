#include "VehicleRenderer.h"
#include "log/mylog.h"
#include <memory>

void VehicleRenderer::create(){
    // build and compile shaders
    // -------------------------
    std::string vs_path = std::string(ROOT_DIR) + std::string("/res/shader/basic.vs");
    std::string fs_path = std::string(ROOT_DIR) + std::string("/res/shader/basic.fs");
    // VehicleShader ourShader(vs_path.c_str(), fs_path.c_str());
    ourShader = new VehicleShader(vs_path.c_str(), fs_path.c_str());
    
    // load models
    // -----------
    std::string path = std::string(ROOT_DIR) + std::string("/res/model/halo/halo.fbx");
    // VehicleMeshInfo ourModel(path);
    ourModel = new VehicleMeshInfo(path);

    // skybox
    std::vector<std::string> faces
    {
        std::string(ROOT_DIR) + std::string("/res/model/skybox/px.png"),
        std::string(ROOT_DIR) + std::string("/res/model/skybox/nx.png"),
        std::string(ROOT_DIR) + std::string("/res/model/skybox/ny.png"),
        std::string(ROOT_DIR) + std::string("/res/model/skybox/py.png"),
        std::string(ROOT_DIR) + std::string("/res/model/skybox/pz.png"),
        std::string(ROOT_DIR) + std::string("/res/model/skybox/nz.png"),
    };

    cubemap.Init(faces);


    mylog(I, "VehicleRenderer::create");
}

void VehicleRenderer::destroy(){
    mylog(I, "VehicleRenderer::destroy");
}

void VehicleRenderer::update(){
    mylog(I, "VehicleRenderer::update");
}

void VehicleRenderer::draw(){
    mylog(I, "VehicleRenderer::draw");
}