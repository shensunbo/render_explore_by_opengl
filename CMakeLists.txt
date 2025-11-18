cmake_minimum_required(VERSION 3.12)

project(Render)

set(CMAKE_CXX_STANDARD 14)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options(-g)

include(${ROOT_DIR}/CMake/set_variable.cmake)
add_subdirectory(src)

message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_BINARY_DIR}")

# Copy resources to build directory
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RESOURCES_DIR} ${CMAKE_BINARY_DIR}/res
    COMMENT "Copying resources to build directory"
)

# Make android_test depend on copying resources
add_dependencies(android_test copy_resources)

# make install
install(DIRECTORY ${RESOURCES_DIR} DESTINATION ${CMAKE_BINARY_DIR})