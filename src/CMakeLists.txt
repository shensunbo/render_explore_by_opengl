
add_executable(render_explore_by_opengl main.cpp)


target_include_directories(render_explore_by_opengl PRIVATE 
    ${COMMON_INC_DIR}
)

target_sources(render_explore_by_opengl PRIVATE ${COMMON_DEP_SRC_FILES})

target_link_directories(render_explore_by_opengl PRIVATE ${COMMON_LIB_DIR})
target_link_libraries(render_explore_by_opengl PRIVATE
    -lassimp
    -limgui
    -lglfw
    -lGL
)

