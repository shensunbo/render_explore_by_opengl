
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
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

target_sources(render_explore_by_opengl PRIVATE 
    ${COMMON_DEP_SRC_FILES}
    platform/Platform.cpp
    rhi/Rhi.h
    core/renderer/Renderer.cpp
    core/camera/VirCamera.cpp
    core/scene/Skybox.cpp
    core/mesh/ModelLoader.cpp
    core/mesh/MeshInfo.cpp
    core/shader/Shader.cpp
    core/mesh/BufferObjectData.cpp
    core/postprocess/FboHandler.cpp
    core/common/DataManager.cpp
    core/common/AnimationHandler.cpp
    core/texture/TextureCache.cpp
    core/renderer/RenderPass.cpp
    core/renderer/RenderGraph.h
    core/common/tool.cpp
)

# for NDK29
target_compile_options(render_explore_by_opengl PRIVATE
    -Wall
    -Wextra
    -fexceptions
    -frtti
)

# Platform-specific linking configuration
if(ANDROID)
    # Android NDK - use system libraries provided by NDK
    target_link_libraries(render_explore_by_opengl PRIVATE
        -lassimp
        log
        GLESv3
    )
else()
    # Desktop/Linux - use local libraries and system GL
    target_link_directories(render_explore_by_opengl PRIVATE ${COMMON_LIB_DIR})
    target_link_libraries(render_explore_by_opengl PRIVATE
        -lassimp
        -limgui
        -lglfw
        -lGL
    )
endif()

# Build android_test executable (Desktop only - requires GLFW)
if(NOT ANDROID)
    add_executable(android_test android_test.cpp)

    target_include_directories(android_test PRIVATE 
        ${COMMON_INC_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/core
    )

    target_link_directories(android_test PRIVATE ${COMMON_LIB_DIR})
    target_link_libraries(android_test PRIVATE
        render_explore_by_opengl
        -lassimp
        -limgui
        -lglfw
        -lGL
    )
endif()
