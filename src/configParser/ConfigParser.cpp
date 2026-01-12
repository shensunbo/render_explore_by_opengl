#include "ConfigParser.h"
#include "log/mylog.h"
#include <fstream>

bool ConfigParser::loadConfigFile(const std::string& config_file_path){
    std::ifstream file(config_file_path);
    if (!file.is_open()) {
        mylog(LogLevel::E, "%s open failed", config_file_path.c_str());
        assert(false);
        return false;
    }

    nlohmann::json js;
    try {
        file >> js;
        file.close();
    } catch (const nlohmann::json::parse_error& e) {
        mylog(LogLevel::E, "error %s ", e.what());
        assert(false);
        return false;
    }

    m_vehicle_name = js.at("vehicleName").get<std::string>();

    // auto door = js.at("rotatable").at("door");
    // parserDoorMeshes(door);

    // auto wheel = js.at("rotatable").at("wheel");
    // parserWheelMeshes(wheel);

    // auto light = js.at("color_adjustable").at("light");
    // parserLightMeshes(light);

    // auto carPaint = js.at("color_adjustable").at("car_paint");
    // parserCarPaintMeshes(carPaint);

    auto texture = js.at("texture");
    parserTextureData(texture);

    // auto disabledMeshes = js.at("disabled_meshes");
    // parserDisabledMeshes(disabledMeshes);

    // auto transparentChassis = js.at("transparent_chassis");
    // parserTransparentChassisMeshes(transparentChassis);

    // auto vehicleParam = js.at("vehicle_params");
    // parserVehicleParam(vehicleParam);

    // meshSetInit();
 
    return true;
}

bool ConfigParser::parserDoorMeshes(const nlohmann::json& door){
    glm::vec3 tmpTranslation = glm::vec3(1.0f);
    glm::vec3 tmpAxis = glm::vec3(0.0f);
    int coef = 1;
    RotateData tmpRotateData{glm::vec3(0.0f), glm::vec3(0.0f), 0, 0, 0};

    {
        std::vector<float> translation = door.at("front_left").at("lcl_translation").get<std::vector<float>>();
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = door.at("front_left").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));

        coef = door.at("front_left").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;
        
        auto components = door.at("front_left").at("components");
        for (auto& component : components) {
            m_fl_door_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
        }
    }

    {
        std::vector<float> translation = door.at("front_right").at("lcl_translation");
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = door.at("front_right").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));

        coef = door.at("front_right").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;

        auto components = door.at("front_right").at("components");
        for (auto& component : components) {
            m_fr_door_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
        }
    }

    {
        std::vector<float> translation = door.at("rear_left").at("lcl_translation"); 
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = door.at("rear_left").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));

        coef = door.at("rear_left").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;

        auto components = door.at("rear_left").at("components");
        for (auto& component : components) {
            m_rl_door_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
        }
    }

    {
        std::vector<float> translation = door.at("rear_right").at("lcl_translation");
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = door.at("rear_right").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));

        coef = door.at("rear_right").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;

        auto components = door.at("rear_right").at("components");
        for (auto& component : components) {
            m_rr_door_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
        }
    }

    {
        std::vector<float> translation = door.at("hood").at("lcl_translation");
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = door.at("hood").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));

        coef = door.at("hood").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;

        auto components = door.at("hood").at("components");
        for (auto& component : components) {
            m_hood_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
        }
    }

    {
        std::vector<float> translation = door.at("trunk").at("lcl_translation");
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = door.at("trunk").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));

        coef = door.at("trunk").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;

        auto components = door.at("trunk").at("components");
        for (auto& component : components) {
            m_trunk_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
        }
    }

    return true;
}

bool ConfigParser::parserWheelMeshes(const nlohmann::json& wheel){
    glm::vec3 tmpTranslation = glm::vec3(1.0f);
    glm::vec3 tmpAxis = glm::vec3(0.0f);
    int coef = 1;
    RotateData tmpRotateData{glm::vec3(0.0f), glm::vec3(0.0f), 0, 0, 0};

    {
        std::vector<float> translation = wheel.at("front_left").at("lcl_translation");
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = wheel.at("front_left").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));
        
        coef = wheel.at("front_left").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;

        std::vector<float> direction_axis = wheel.at("front_left").at("direction_rotate_axis").get<std::vector<float>>();

        auto components = wheel.at("front_left").at("components");
        for (auto& component : components) {
            m_fl_wheel_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
            front_wheel_direction_axis[component] = glm::vec3(direction_axis.at(0), direction_axis.at(1), direction_axis.at(2));
        }
    }

    {
        std::vector<float> translation = wheel.at("front_right").at("lcl_translation");
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = wheel.at("front_right").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));

        coef = wheel.at("front_left").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;

        std::vector<float> direction_axis = wheel.at("front_right").at("direction_rotate_axis").get<std::vector<float>>();

        auto components = wheel.at("front_right").at("components");
        for (auto& component : components) {
            m_fr_wheel_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
            front_wheel_direction_axis[component] = glm::vec3(direction_axis.at(0), direction_axis.at(1), direction_axis.at(2));
        }
    }

    {
        std::vector<float> translation = wheel.at("rear_left").at("lcl_translation");
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = wheel.at("rear_left").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));

        coef = wheel.at("front_left").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;

        auto components = wheel.at("rear_left").at("components");
        for (auto& component : components) {
            m_rl_wheel_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
        }
    }

    {
        std::vector<float> translation = wheel.at("rear_right").at("lcl_translation");
        tmpTranslation = glm::vec3(translation.at(0), translation.at(1), translation.at(2));

        std::vector<float> axis = wheel.at("rear_right").at("rotate_axis").get<std::vector<float>>();
        tmpAxis = glm::vec3(axis.at(0), axis.at(1), axis.at(2));

        coef = wheel.at("front_left").at("coef").get<int>();

        tmpRotateData.translation = tmpTranslation;
        tmpRotateData.axis = tmpAxis;
        tmpRotateData.coef = coef;

        auto components = wheel.at("rear_right").at("components");
        for (auto& component : components) {
            m_rr_wheel_meshes.insert(component);
            m_rotatable_meshes_transforms[component] = tmpRotateData;
        }
    }

    return true;
}

bool ConfigParser::parserLightMeshes(const nlohmann::json& light){
    std::array<glm::vec3, 2> tmpColor = {glm::vec3(0.0f), glm::vec3(0.0f)};
   {
        std::vector<float> off_color = light.at("amber").at("off_color");
        tmpColor[0] = glm::vec3(off_color.at(0), off_color.at(1), off_color.at(2));
        std::vector<float> on_color = light.at("amber").at("on_color");
        tmpColor[1] = glm::vec3(on_color.at(0), on_color.at(1), on_color.at(2));

        auto components = light.at("amber").at("components");
        for (auto& component : components) {
            m_amber_light_meshes.insert(component);
            m_light_meshes_color[component] = tmpColor;
        }
    }

    {
        std::vector<float> off_color = light.at("red").at("off_color");
        tmpColor[0] = glm::vec3(off_color.at(0), off_color.at(1), off_color.at(2));
        std::vector<float> on_color = light.at("red").at("on_color");
        tmpColor[1] = glm::vec3(on_color.at(0), on_color.at(1), on_color.at(2));

        auto components = light.at("red").at("components");
        for (auto& component : components) {
            m_red_light_meshes.insert(component);
            m_light_meshes_color[component] = tmpColor;
        }
    }

    {
        std::vector<float> off_color = light.at("white").at("off_color");
        tmpColor[0] = glm::vec3(off_color.at(0), off_color.at(1), off_color.at(2));
        std::vector<float> on_color = light.at("white").at("on_color");
        tmpColor[1] = glm::vec3(on_color.at(0), on_color.at(1), on_color.at(2));
    
        auto components = light.at("white").at("components");
        for (auto& component : components) {
            m_white_light_meshes.insert(component);
            m_light_meshes_color[component] = tmpColor;
        }
    }

    return true;
}

bool ConfigParser::parserCarPaintMeshes(const nlohmann::json& carPaint){
    std::string materialName = carPaint.at("material").get<std::string>();
    std::vector<float> defaultColor = carPaint.at("color").at("default").get<std::vector<float>>();

    m_car_paint_color[materialName]  = glm::vec3(defaultColor.at(0), defaultColor.at(1), defaultColor.at(2));;
    m_current_car_paint = std::make_pair(materialName, m_car_paint_color[materialName]);
    return true;
}

bool ConfigParser::parserTextureData(const nlohmann::json& texture){
    if(texture.contains("diffuse")){
        mylog(LogLevel::I, "---> diffuse texture");
        std::vector<std::string> fileName = texture.at("diffuse").at("file_list").get<std::vector<std::string>>();
        auto diffuseNode = texture.at("diffuse");

        for (auto& file : fileName) {
            m_texture_paths.insert(file);
            auto components = diffuseNode.at(file).at("components");
            for (auto& component : components) {
                std::string meshname = component.at("mesh_name");
                std::string material = component.at("material_name");
                m_texture_data[std::make_pair(meshname, material)].diffuse = file;
            }
        }
    }else{
        mylog(LogLevel::I, "no diffuse texture in config file");
    }

    if(texture.contains("specular")){
        mylog(LogLevel::I, "---> specular texture");
        std::vector<std::string> fileName = texture.at("specular").at("file_list").get<std::vector<std::string>>();
        auto specularNode = texture.at("specular");

        for (auto& file : fileName) {
            m_texture_paths.insert(file);
            auto components = specularNode.at(file).at("components");
            for (auto& component : components) {
                std::string meshname = component.at("mesh_name");
                std::string material = component.at("material_name");
                m_texture_data[std::make_pair(meshname, material)].specular = file;
            }
        }

    } else{
        mylog(LogLevel::I, "---> no specular texture");
    }

    if(texture.contains("normal")){
        mylog(LogLevel::I, "---> normal texture");
        std::vector<std::string> fileName = texture.at("normal").at("file_list").get<std::vector<std::string>>();
        auto normalNode = texture.at("normal");

        for (auto& file : fileName) {
            m_texture_paths.insert(file);
            auto components = normalNode.at(file).at("components");
            for (auto& component : components) {
                std::string meshname = component.at("mesh_name");
                std::string material = component.at("material_name");
                m_texture_data[std::make_pair(meshname, material)].normal = file;
            }
        }

    } else{
        mylog(LogLevel::I, "---> no normal texture");
    }

    if(texture.contains("ao")){
        mylog(LogLevel::I, "---> ao texture");
        std::vector<std::string> fileName = texture.at("ao").at("file_list").get<std::vector<std::string>>();
        auto aoNode = texture.at("ao");

        for (auto& file : fileName) {
            m_texture_paths.insert(file);
            auto components = aoNode.at(file).at("components");
            for (auto& component : components) {
                std::string meshname = component.at("mesh_name");
                std::string material = component.at("material_name");
                m_texture_data[std::make_pair(meshname, material)].ao = file;
            }
        }
    } else{
        mylog(LogLevel::I, "---> no ao texture");
    }

    if(texture.contains("alpha")){
        mylog(LogLevel::I, "---> alpha texture");
        std::vector<std::string> fileName = texture.at("alpha").at("file_list").get<std::vector<std::string>>();
        auto alphaNode = texture.at("alpha");

        for (auto& file : fileName) {
            m_texture_paths.insert(file);
            auto components = alphaNode.at(file).at("components");
            for (auto& component : components) {
                std::string meshname = component.at("mesh_name");
                std::string material = component.at("material_name");
                m_texture_data[std::make_pair(meshname, material)].alpha = file;
            }
        }
    } else{
        mylog(LogLevel::I, "---> no alpha texture");
    }

    if(texture.contains("roughness")){
        mylog(LogLevel::I, "---> roughness texture");
        std::vector<std::string> fileName = texture.at("roughness").at("file_list").get<std::vector<std::string>>();
        auto roughnessNode = texture.at("roughness");

        for (auto& file : fileName) {
            m_texture_paths.insert(file);
            auto components = roughnessNode.at(file).at("components");
            for (auto& component : components) {
                std::string meshname = component.at("mesh_name");
                std::string material = component.at("material_name");
                m_texture_data[std::make_pair(meshname, material)].roughness = file;
            }
        }
    } else{
        mylog(LogLevel::I, "---> no roughness texture");
    }

    if(texture.contains("metallic")){
        mylog(LogLevel::I, "---> metallic texture");
        std::vector<std::string> fileName = texture.at("metallic").at("file_list").get<std::vector<std::string>>();
        auto metallicNode = texture.at("metallic");

        for (auto& file : fileName) {
            m_texture_paths.insert(file);
            auto components = metallicNode.at(file).at("components");
            for (auto& component : components) {
                std::string meshname = component.at("mesh_name");
                std::string material = component.at("material_name");
                m_texture_data[std::make_pair(meshname, material)].metallic = file;
            }
        }
    } else{
        mylog(LogLevel::I, "---> no metallic texture");
    }


    mylog(LogLevel::I, "total texture files: %d", m_texture_paths.size());

    return true;
}

bool ConfigParser::parserDisabledMeshes(const nlohmann::json& disabledMeshes){
    auto components = disabledMeshes.at("mesh_prefix");
    for (auto& component : components) {
        m_disabledMeshes.insert(component);
    }
    return true;
}

bool ConfigParser::parserTransparentChassisMeshes(const nlohmann::json& transparentChassis){
    auto trans = transparentChassis.at("transparent");
    for (auto component : trans) {
        m_transparent_chassis_meshes.insert(component);
    }

    auto notTrans = transparentChassis.at("not_transparent");
    for (auto component : notTrans) {
        m_not_transparent_chassis_meshes.insert(component);
    }

    auto hide = transparentChassis.at("hide");
    for (auto component : hide) {
        m_hide_chassis_meshes.insert(component);
    }

    return true;
}

bool ConfigParser::parserVehicleParam(const nlohmann::json& vehicleParam){
    m_vehicle_param.min_x = vehicleParam.at("min_x").get<float>();
    m_vehicle_param.max_x = vehicleParam.at("max_x").get<float>();
    m_vehicle_param.min_y = vehicleParam.at("min_y").get<float>();
    m_vehicle_param.max_y = vehicleParam.at("max_y").get<float>();
    m_vehicle_param.min_z = vehicleParam.at("min_z").get<float>();
    m_vehicle_param.max_z = vehicleParam.at("max_z").get<float>();
    m_vehicle_param.coordinate_scale_to_mm = vehicleParam.at("coordinate_scale_to_mm").get<unsigned int>();

    m_vehicle_param.scale_x_3d = vehicleParam.at("layout").at("scale_x").get<float>();
    m_vehicle_param.scale_y_3d = vehicleParam.at("layout").at("scale_y").get<float>();
    m_vehicle_param.scale_z_3d = vehicleParam.at("layout").at("scale_z").get<float>();
    m_vehicle_param.rotation_axis_x_3d = vehicleParam.at("layout").at("rotation_axis_x").get<float>();
    m_vehicle_param.rotation_axis_y_3d = vehicleParam.at("layout").at("rotation_axis_y").get<float>();
    m_vehicle_param.rotation_axis_z_3d = vehicleParam.at("layout").at("rotation_axis_z").get<float>();
    m_vehicle_param.translation_x_3d = vehicleParam.at("layout").at("translation_x").get<float>();
    m_vehicle_param.translation_y_3d = vehicleParam.at("layout").at("translation_y").get<float>();
    m_vehicle_param.translation_z_3d = vehicleParam.at("layout").at("translation_z").get<float>();

    return true;
}

void ConfigParser::meshSetInit(){
    //m_rotatableMeshes
    m_rotatableMeshes.insert(m_fl_door_meshes.begin(), m_fl_door_meshes.end());
    m_rotatableMeshes.insert(m_fr_door_meshes.begin(), m_fr_door_meshes.end());
    m_rotatableMeshes.insert(m_rl_door_meshes.begin(), m_rl_door_meshes.end());
    m_rotatableMeshes.insert(m_rr_door_meshes.begin(), m_rr_door_meshes.end());
    m_rotatableMeshes.insert(m_hood_meshes.begin(), m_hood_meshes.end());
    m_rotatableMeshes.insert(m_trunk_meshes.begin(), m_trunk_meshes.end());
    m_rotatableMeshes.insert(m_fl_wheel_meshes.begin(), m_fl_wheel_meshes.end());
    m_rotatableMeshes.insert(m_fr_wheel_meshes.begin(), m_fr_wheel_meshes.end());
    m_rotatableMeshes.insert(m_rl_wheel_meshes.begin(), m_rl_wheel_meshes.end());
    m_rotatableMeshes.insert(m_rr_wheel_meshes.begin(), m_rr_wheel_meshes.end());

    //m_lightMeshes
    m_light_meshes.insert(m_amber_light_meshes.begin(), m_amber_light_meshes.end());
    m_light_meshes.insert(m_red_light_meshes.begin(), m_red_light_meshes.end());
    m_light_meshes.insert(m_white_light_meshes.begin(), m_white_light_meshes.end());

    //m_adjustableMeshes
    m_adjustableMeshes.insert(m_rotatableMeshes.begin(), m_rotatableMeshes.end());
    m_adjustableMeshes.insert(m_light_meshes.begin(), m_light_meshes.end());
}


bool ConfigParser::isAdjustableMesh(const std::string& mesh){
    return m_adjustableMeshes.find(mesh) != m_adjustableMeshes.end();
}

bool ConfigParser::isRotatableMesh(const std::string& mesh){
    return m_rotatableMeshes.find(mesh) != m_rotatableMeshes.end();
}

bool ConfigParser::isLightMesh(const std::string& mesh){
    return m_light_meshes.find(mesh) != m_light_meshes.end();
}

bool ConfigParser::isCarPaintMesh(const std::string& mesh){
    return m_car_paint_meshes.find(mesh) != m_car_paint_meshes.end();
}

bool ConfigParser::isFrontWheelMesh(const std::string& mesh){
    return m_fl_wheel_meshes.find(mesh) != m_fl_wheel_meshes.end() ||
           m_fr_wheel_meshes.find(mesh) != m_fr_wheel_meshes.end();
}

bool ConfigParser::isDisabledMesh(const std::string& mesh){
    return m_disabledMeshes.find(mesh) != m_disabledMeshes.end();
}

bool ConfigParser::isTransparentInChassis(const std::string& mesh){
    return m_transparent_chassis_meshes.find(mesh) != m_transparent_chassis_meshes.end();
}

bool ConfigParser::isHideInChassis(const std::string& mesh){
    return m_hide_chassis_meshes.find(mesh) != m_hide_chassis_meshes.end();
}

/* @brief check if the mesh needs texture
 * @param mesh: mesh name
 * @param material: material name
 * @return true if the mesh needs texture, otherwise false
 */
bool ConfigParser::needTexture(const std::string& mesh, const std::string& material){
    return m_texture_data.find(std::make_pair(mesh, material)) != m_texture_data.end() ||
        m_texture_data.find(std::make_pair(mesh, "")) != m_texture_data.end() ;
}

TextureData ConfigParser::getTextureData(const std::string& mesh, const std::string& material){
    if(m_texture_data.find(std::make_pair(mesh, "")) != m_texture_data.end()){
        return m_texture_data[std::make_pair(mesh, "")];
    }else{
        return m_texture_data[std::make_pair(mesh, material)];
    }
}

RotateData ConfigParser::getRotatableMeshData(const std::string& mesh){
    return m_rotatable_meshes_transforms[mesh];
}

glm::vec3 ConfigParser::getLightMeshColor(const std::string& mesh, bool state){
    if (state) {
        return m_light_meshes_color[mesh][1];
    } else {
        return m_light_meshes_color[mesh][0];
    }
}

glm::vec3 ConfigParser::getFrontWheelDirectionAxis(const std::string& mesh){
    return front_wheel_direction_axis[mesh];
}

void ConfigParser::updateFlTargetDoorAngle(int angle){
    for (auto& component : m_fl_door_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
    }
}

void ConfigParser::updateFrTargetDoorAngle(int angle){
    for (auto& component : m_fr_door_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
    }
}

void ConfigParser::updateRlTargetDoorAngle(int angle){
    for (auto& component : m_rl_door_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
    }
}

void ConfigParser::updateRrTargetDoorAngle(int angle){
    for (auto& component : m_rr_door_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
    }
}

void ConfigParser::updateHoodTargetAngle(int angle){
    for (auto& component : m_hood_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
    }
}

void ConfigParser::updateTrunkTargetAngle(int angle){
    for (auto& component : m_trunk_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
    }
}

void ConfigParser::updateFlCurrentDoorAngle(int step){
    for (auto& component : m_fl_door_meshes) {
        auto& transform = m_rotatable_meshes_transforms[component];

        if (transform.currentAngle < transform.targetAngle) {
            transform.currentAngle = std::min(transform.currentAngle + step, transform.targetAngle);
        } else if (transform.currentAngle > transform.targetAngle) {
            transform.currentAngle = std::max(transform.currentAngle - step, transform.targetAngle);
        }
    }
}

void ConfigParser::updateFrCurrentDoorAngle(int step){
    for (auto& component : m_fr_door_meshes) {
        auto& transform = m_rotatable_meshes_transforms[component];

        if (transform.currentAngle < transform.targetAngle) {
            transform.currentAngle = std::min(transform.currentAngle + step, transform.targetAngle);
        } else if (transform.currentAngle > transform.targetAngle) {
            transform.currentAngle = std::max(transform.currentAngle - step, transform.targetAngle);
        }
    }
}

void ConfigParser::updateRlCurrentDoorAngle(int step){
    for (auto& component : m_rl_door_meshes) {
        auto& transform = m_rotatable_meshes_transforms[component];

        if (transform.currentAngle < transform.targetAngle) {
            transform.currentAngle = std::min(transform.currentAngle + step, transform.targetAngle);
        } else if (transform.currentAngle > transform.targetAngle) {
            transform.currentAngle = std::max(transform.currentAngle - step, transform.targetAngle);
        }
    }
}

void ConfigParser::updateRrCurrentDoorAngle(int step){
    for (auto& component : m_rr_door_meshes) {
        auto& transform = m_rotatable_meshes_transforms[component];

        if (transform.currentAngle < transform.targetAngle) {
            transform.currentAngle = std::min(transform.currentAngle + step, transform.targetAngle);
        } else if (transform.currentAngle > transform.targetAngle) {
            transform.currentAngle = std::max(transform.currentAngle - step, transform.targetAngle);
        }
    }
}

void ConfigParser::updateHoodCurrentAngle(int step){
    for (auto& component : m_hood_meshes) {
        auto& transform = m_rotatable_meshes_transforms[component];

        if (transform.currentAngle < transform.targetAngle) {
            transform.currentAngle = std::min(transform.currentAngle + step, transform.targetAngle);
        } else if (transform.currentAngle > transform.targetAngle) {
            transform.currentAngle = std::max(transform.currentAngle - step, transform.targetAngle);
        }
    }
}

void ConfigParser::updateTrunkCurrentAngle(int step){
    for (auto& component : m_trunk_meshes) {
        auto& transform = m_rotatable_meshes_transforms[component];

        if (transform.currentAngle < transform.targetAngle) {
            transform.currentAngle = std::min(transform.currentAngle + step, transform.targetAngle);
        } else if (transform.currentAngle > transform.targetAngle) {
            transform.currentAngle = std::max(transform.currentAngle - step, transform.targetAngle);
        }
    }
}

void ConfigParser::updateWheelAngle(int angle){
    for (auto& component : m_fl_wheel_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
        m_rotatable_meshes_transforms[component].currentAngle = angle;
    }

    for (auto& component : m_fr_wheel_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
        m_rotatable_meshes_transforms[component].currentAngle = angle;
    }

    for (auto& component : m_rl_wheel_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
        m_rotatable_meshes_transforms[component].currentAngle = angle;
    }

    for (auto& component : m_rr_wheel_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
        m_rotatable_meshes_transforms[component].currentAngle = angle;
    }
}

void ConfigParser::updateFlWheelAngle(int angle){
    for (auto& component : m_fl_wheel_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
        m_rotatable_meshes_transforms[component].currentAngle = angle;
    }
}

void ConfigParser::updateFrWheelAngle(int angle){
    for (auto& component : m_fr_wheel_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
        m_rotatable_meshes_transforms[component].currentAngle = angle;
    }
}

void ConfigParser::updateRlWheelAngle(int angle){
    for (auto& component : m_rl_wheel_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
        m_rotatable_meshes_transforms[component].currentAngle = angle;
    }
}

void ConfigParser::updateRrWheelAngle(int angle){
    for (auto& component : m_rr_wheel_meshes) {
        m_rotatable_meshes_transforms[component].targetAngle = angle;
        m_rotatable_meshes_transforms[component].currentAngle = angle;
    }
}

void ConfigParser::updateRotateMeshTranslation(const std::string& mesh, const glm::vec3& translation){
    m_rotatable_meshes_transforms[mesh].translation = translation;
}
