#include "VehicleRenderer.h"
#include "log/mylog.h"
#include <memory>

void VehicleRenderer::create(unsigned int width, unsigned int height, const std::string& resourcePrefix){

    // Add trailing slash to resource prefix if not empty
    std::string prefix = resourcePrefix;
    if (!prefix.empty() && prefix.back() != '/') {
        prefix += '/';
    }

    std::string cfgPath =  prefix + std::string("res/model/halo/vehicle_info.json");
    mylog(LogLevel::I, "Loading config file: %s", cfgPath.c_str());
    cfgParser.loadConfigFile(cfgPath);

    // build and compile shaders
    // -------------------------
    std::string vs_path =  prefix + std::string("res/shader/with_texture.vs");
    std::string fs_path =  prefix + std::string("res/shader/with_texture_test.fs");
    // VehicleShader ourShader(vs_path.c_str(), fs_path.c_str());
    ourShader = new VehicleShader(vs_path.c_str(), fs_path.c_str());
    
    // load models
    // -----------
    std::string path =  prefix + std::string("res/model/halo/halo.fbx");
    // std::string path =  std::string("res/model/ford/vehicle.fbx");

    // VehicleMeshInfo ourModel(path);
    ourModel = new VehicleMeshInfo(path, cfgParser);

    for(auto& it : ourModel->meshes) {
        unsigned int blockIndex = ourShader->getBlockIndex("MaterialBlock");
        ourShader->uniformBlockBind(blockIndex, 0);
        it.updateUbo(it.mUboMat);
        mylog(LogLevel::D, "VehicleRenderer::create: mesh name: %s, MaterialName: %s", it.meshName.c_str(), it.mMaterial.MaterialName.c_str());
    }

    // skybox
    std::vector<std::string> faces
    {
         prefix + std::string("res/model/skybox/px.png"),
         prefix + std::string("res/model/skybox/nx.png"),
         prefix + std::string("res/model/skybox/py.png"),
         prefix + std::string("res/model/skybox/ny.png"),
         prefix + std::string("res/model/skybox/pz.png"),
         prefix + std::string("res/model/skybox/nz.png"),
    };

    unsigned int skyboxBindID = ourModel->getMaxTextureID() + 1;
    cubemap = std::make_shared<Skybox>(skyboxBindID, prefix + std::string("res/shader/skybox.vs"), prefix + std::string("res/shader/skybox.fs"));
    cubemap->Init(faces);

    // TODO
    fbo_ = std::make_shared<FboHandler>(width, height, prefix + std::string("res/shader/fbo_rect.vs"), prefix + std::string("res/shader/fbo_rect.fs"));
    fbo_->init();

    mylog(LogLevel::I, "VehicleRenderer::create");
}

void VehicleRenderer::destroy(){
    mylog(LogLevel::I, "VehicleRenderer::destroy");

    delete ourShader;
    delete ourModel;
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

            ourShader->setBool((uniform + "_load"), true);
            ourShader->setBool("textureLoad", true);
        }

        // draw mesh
        v.bindVao();
        v.bindUbo();

        glDrawElements(GL_TRIANGLES, v.getIndicesSize(), GL_UNSIGNED_INT, 0);

        ourShader->setBool("textureLoad", false);
    }

    // mylog(LogLevel::I, "VehicleRenderer::draw");
}