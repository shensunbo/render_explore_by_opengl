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

    delete ourShader;
    delete ourModel;
}

void VehicleRenderer::update(){
    mylog(I, "VehicleRenderer::update");
}

void VehicleRenderer::draw(){
    for(auto& v :  ourModel->meshes){
        ourShader->setVec3("materialDiffuseColor", v.mMaterial.diffuseColor);
        ourShader->setVec3("materialSpecularColor", v.mMaterial.specularColor);
        ourShader->setVec3("materialAmbientColor", v.mMaterial.ambientColor);
        ourShader->setFloat("Shininess", v.mMaterial.shininess);
        ourShader->setFloat("ShininessStrength", v.mMaterial.ShininessStrength);
        ourShader->setFloat("Opacity", v.mMaterial.Opacity);

        // draw mesh
        v.bindVao();

        glDrawElements(GL_TRIANGLES, v.getIndicesSize(), GL_UNSIGNED_INT, 0);
    }

    // mylog(I, "VehicleRenderer::draw");
}