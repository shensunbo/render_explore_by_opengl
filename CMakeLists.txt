cmake_minimum_required(VERSION 3.12)

project(Render)

set(CMAKE_CXX_STANDARD 14)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options(-g)

add_definitions(-DROOT_DIR="${ROOT_DIR}")

include(${ROOT_DIR}/CMake/set_variable.cmake)
add_subdirectory(src)

message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_BINARY_DIR}")

# make install
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res )
install(DIRECTORY ${RESOURCES_DIR} DESTINATION ${CMAKE_BINARY_DIR})