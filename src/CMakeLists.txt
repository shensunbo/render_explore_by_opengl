
add_executable(render_explore_by_opengl refactor_test.cpp)

target_sources(render_explore_by_opengl PRIVATE 
    configParser/ConfigParser.cpp
)

target_include_directories(render_explore_by_opengl PRIVATE 
    ${COMMON_INC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    core/refactor
)

target_sources(render_explore_by_opengl PRIVATE 
    ${COMMON_DEP_SRC_FILES}
    core/refactor/VehicleRenderer.cpp
    core/refactor/VehicleVirCamera.cpp
    core/refactor/Skybox.cpp
    core/refactor/ModelLoader.cpp
    core/refactor/VehicleMeshInfo.cpp
    core/refactor/VehicleShader.cpp
    core/refactor/BufferObjectData.cpp
)

target_link_directories(render_explore_by_opengl PRIVATE ${COMMON_LIB_DIR})
target_link_libraries(render_explore_by_opengl PRIVATE
    -lassimp
    -limgui
    -lglfw
    -lGL
)

