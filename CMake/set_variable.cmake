set(COMMON_INC_DIR 
    ${ROOT_DIR}/include
    ${ROOT_DIR}/dependency
    ${ROOT_DIR}/dependency/stb
    ${ROOT_DIR}/dependency/imgui
    ${ROOT_DIR}/dependency/assimp/include
    ${ROOT_DIR}/dependency/glad/include
)

set(COMMON_LIB_DIR 
    ${ROOT_DIR}/dependency/assimp/lib
)

file(GLOB IMGUI_SRC_FILES ${ROOT_DIR}/dependency/imgui/*.cpp)
set(COMMON_DEP_SRC_FILES 
    ${IMGUI_SRC_FILES}
    ${ROOT_DIR}/dependency/glad/src/glad.c
)