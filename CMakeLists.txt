cmake_minimum_required(VERSION 3.15)
project(Cube-Maker)
set(CMAKE_CXX_STANDARD 17)

# OpenGL
find_package(OpenGL REQUIRED)

# GLAD
include_directories(external/glad/include)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)
include_directories(external/glfw)

# GLM
add_subdirectory(external/glm)
include_directories(external/glm)

# For include directory
include_directories(include)
file(COPY include DESTINATION ${CMAKE_BINARY_DIR})

# Project
add_executable(
        ${PROJECT_NAME}
        external/glad/src/glad.c
        main.cpp
)
target_link_libraries(
        ${PROJECT_NAME}
        OpenGL::GL
        glfw
)
