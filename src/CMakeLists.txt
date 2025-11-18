
# add_executable(render_explore_by_opengl refactor_test.cpp)

add_library(render_explore_by_opengl SHARED
    renderer_api.cpp
)

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
    core/refactor/FboHandler.cpp
    core/refactor/DataManager.cpp
    core/refactor/AnimationHandler.cpp
)

target_link_directories(render_explore_by_opengl PRIVATE ${COMMON_LIB_DIR})
target_link_libraries(render_explore_by_opengl PRIVATE
    -lassimp
    -limgui
    -lglfw
    -lGL
)

# Build android_test executable
add_executable(android_test android_test.cpp)

target_include_directories(android_test PRIVATE 
    ${COMMON_INC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    core/refactor
)

target_link_directories(android_test PRIVATE ${COMMON_LIB_DIR})
target_link_libraries(android_test PRIVATE
    render_explore_by_opengl
    -lassimp
    -limgui
    -lglfw
    -lGL
)
