
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
    platform/Platform.cpp
    rhi/Rhi.h
    core/refactor/Renderer.cpp
    core/refactor/VirCamera.cpp
    core/refactor/Skybox.cpp
    core/refactor/ModelLoader.cpp
    core/refactor/MeshInfo.cpp
    core/refactor/Shader.cpp
    core/refactor/BufferObjectData.cpp
    core/refactor/FboHandler.cpp
    core/refactor/DataManager.cpp
    core/refactor/AnimationHandler.cpp
    core/refactor/TextureCache.cpp
    core/refactor/RenderPass.cpp
    core/refactor/RenderGraph.h
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
endif()
