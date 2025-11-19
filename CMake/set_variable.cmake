set(COMMON_INC_DIR 
    ${ROOT_DIR}/include
    ${ROOT_DIR}/dependency
    ${ROOT_DIR}/dependency/stb
    ${ROOT_DIR}/dependency/imgui/include
    ${ROOT_DIR}/dependency/glad/include
    ${ROOT_DIR}/dependency/glm
    ${ROOT_DIR}/dependency/json
)

# Platform-specific include and lib directories
if(ANDROID)
    list(APPEND COMMON_INC_DIR 
        ${ROOT_DIR}/dependency/assimp/android/include
    )
    set(COMMON_LIB_DIR 
        ${ROOT_DIR}/dependency/assimp/android
    )
    
    # Add library search path for NDK
    link_directories(${COMMON_LIB_DIR})
else()
    # Desktop - adjust paths as needed for your system
    # Uncomment if you have assimp in dependency directory
    # list(APPEND COMMON_INC_DIR 
    #     ${ROOT_DIR}/dependency/assimp/include
    # )
    # set(COMMON_LIB_DIR 
    #     ${ROOT_DIR}/dependency/assimp/lib
    #     ${ROOT_DIR}/dependency/imgui/lib
    # )
endif()

# set(COMMON_DEP_SRC_FILES 
#     ${ROOT_DIR}/dependency/glad/src/glad.c
# )