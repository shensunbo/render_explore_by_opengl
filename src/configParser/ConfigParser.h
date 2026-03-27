#pragma once
#include <string>
#include <set>
#include <unordered_map>
#include <array>
#include <vector>
#include <utility>
#include "glm/glm.hpp"
#include "json.hpp"

/* @brief RotateData struct
 * @param axis: the axis of rotation
 * @param translation: use to do world to local transformation
 * @param angle: the angle of rotation
 * @param coef: use to adjust the angle rotate direction 
 */

typedef struct RotateData {
    glm::vec3 axis;
    glm::vec3 translation;
    int targetAngle;
    int currentAngle;
    int coef;
} RotateData;

/* @brief TextureData struct
 * @param diffuse: the diffuse texture path
 * @param specular: the specular texture path
 * @param normal: the normal texture path
 * @param ao: the Ambient Occlusion texture path
 * @param alpha: the alpha texture path
 * @param roughness: the roughness texture path
 * @param metallic: the metallic texture path
 */
typedef struct TextureData {
    std::string diffuse;
    std::string specular;
    std::string normal;
    std::string ao;
    std::string alpha;
    std::string roughness;
    std::string metallic;
} TextureData;

typedef struct Param {
    // min and max value of model coordinate
    float min_x;
    float max_x;
    float min_y;
    float max_y;
    float min_z;
    float max_z;
    unsigned int coordinate_scale_to_mm;

    float scale_x_3d;
    float scale_y_3d;
    float scale_z_3d;
    float translation_x_3d;
    float translation_y_3d;
    float translation_z_3d;
    float rotation_axis_x_3d;
    float rotation_axis_y_3d;
    float rotation_axis_z_3d;
    float rotation_axis_x_bev;
    float rotation_axis_y_bev;
    float rotation_axis_z_bev;
    float translation_x_bev;
    float translation_y_bev;
    float translation_z_bev;
    //skybox
    float rotation_axis_x_skybox_bev;
    float rotation_axis_y_skybox_bev;
    float rotation_axis_z_skybox_bev;
    float translation_x_skybox_bev;
    float translation_y_skybox_bev;
    float translation_z_skybox_bev;
    float rotation_axis_x_skybox_3d;
    float rotation_axis_y_skybox_3d;
    float rotation_axis_z_skybox_3d;
    float translation_x_skybox_3d;
    float translation_y_skybox_3d;
    float translation_z_skybox_3d;
} Param;

class ConfigParser {
public:
    bool loadConfigFile(const std::string& config_file_path, std::set<std::string>& texture_paths_set);

    bool isAdjustableMesh(const std::string& mesh);
    bool isRotatableMesh(const std::string& mesh);
    bool isLightMesh(const std::string& mesh);
    bool isCarPaintMesh(const std::string& mesh);
    bool isFrontWheelMesh(const std::string& mesh);
    bool isFrontWheelCalipersMesh(const std::string& mesh);
    bool isDisabledMesh(const std::string& mesh);
    
    // transparent chassis mode
    bool isTransparentInChassis(const std::string& mesh);
    bool isHideInChassis(const std::string& mesh);

    bool needTexture(const std::string& mesh, const std::string& material);
    bool haveAlphaTexture(const std::string& mesh, const std::string& material);

    std::string getName() { return m_name; }
    RotateData getRotatableMeshData(const std::string& mesh);
    glm::vec3 getLightMeshColor(const std::string& mesh, bool state);
    glm::vec3 getFrontWheelDirectionAxis(const std::string& mesh);
    TextureData getTextureData(const std::string& mesh, const std::string& material);
    std::set<std::string> getDisabledMeshes() {return m_disabledMeshes; }
    std::pair <std::string, glm::vec3> getCurrentCarPaint() { return m_current_car_paint; }
    std::set<std::string> getHideMeshesInChassis() { return m_hide_chassis_meshes; }
    Param getParam() { return m_param; }
    // const std::set<std::string>& getTexturePaths() const { return m_texture_paths; }

    void updateFlTargetDoorAngle(int angle);
    void updateFrTargetDoorAngle(int angle);
    void updateRlTargetDoorAngle(int angle);
    void updateRrTargetDoorAngle(int angle);
    void updateHoodTargetAngle(int angle);
    void updateTrunkTargetAngle(int angle);
    void updateFlCurrentDoorAngle(int step);
    void updateFrCurrentDoorAngle(int step);
    void updateRlCurrentDoorAngle(int step);
    void updateRrCurrentDoorAngle(int step);
    void updateHoodCurrentAngle(int step);
    void updateTrunkCurrentAngle(int step);
    void updateWheelAngle(int angle);
    void updateFlWheelAngle(int angle);
    void updateFrWheelAngle(int angle);
    void updateRlWheelAngle(int angle);
    void updateRrWheelAngle(int angle);
    void updateRotateMeshTranslation(const std::string& mesh, const glm::vec3& translation);

private:
    bool parserDoorMeshes(const nlohmann::json& door);
    bool parserWheelMeshes(const nlohmann::json& wheel);
    bool parserLightMeshes(const nlohmann::json& light);
    bool parserCarPaintMeshes(const nlohmann::json& carPaint);
    bool parserTextureData(const nlohmann::json& texture, std::set<std::string>& texture_paths_set);
    bool parserDisabledMeshes(const nlohmann::json& disabledMeshes);
    bool parserTransparentChassisMeshes(const nlohmann::json& transparentChassis);
    bool parserParam(const nlohmann::json& param);
    void meshSetInit();
private:
    std::string m_name;
    std::set<std::string> m_adjustableMeshes;
    std::set<std::string> m_rotatableMeshes;
    std::set<std::string> m_disabledMeshes;

    std::set<std::string> m_fl_door_meshes;
    std::set<std::string> m_fr_door_meshes;
    std::set<std::string> m_rl_door_meshes;
    std::set<std::string> m_rr_door_meshes;
    std::set<std::string> m_hood_meshes;
    std::set<std::string> m_trunk_meshes;
    std::set<std::string> m_fl_wheel_meshes;
    std::set<std::string> m_fr_wheel_meshes;
    std::set<std::string> m_rl_wheel_meshes;
    std::set<std::string> m_rr_wheel_meshes;
    std::set<std::string> m_f_caliper_meshes;
    std::unordered_map<std::string, RotateData> m_rotatable_meshes_transforms;
    std::unordered_map<std::string, glm::vec3> front_wheel_direction_axis;

    std::set<std::string> m_light_meshes;
    std::set<std::string> m_amber_light_meshes;
    std::set<std::string> m_red_light_meshes;
    std::set<std::string> m_white_light_meshes;
    std::unordered_map<std::string, std::array<glm::vec3, 2>> m_light_meshes_color;

    std::set<std::string> m_car_paint_meshes;

    std::unordered_map<std::string, glm::vec3> m_car_paint_color;
    std::pair <std::string, glm::vec3> m_current_car_paint;

    //texture, first meshname, second material; if material is "", the texture is for all material
    std::map<std::pair<std::string, std::string>, TextureData> m_texture_data;
    // std::set<std::string> m_texture_paths;
    std::string m_path_prefix;

    // transparent chassis mode
    std::set<std::string> m_transparent_chassis_meshes;
    std::set<std::string> m_not_transparent_chassis_meshes;
    std::set<std::string> m_hide_chassis_meshes;

    Param m_param;
};