
cmake_minimum_required(VERSION 3.20)

project(lightmap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
find_package(glfw3 3.3 CONFIG QUIET)

if(NOT glfw3_FOUND)
    message(STATUS "glfw3 не найден, скачиваю с GitHub")

    include(FetchContent)

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW ON
    )

    FetchContent_MakeAvailable(glfw)
endif()

# Dear ImGui
include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(imgui)

# ImGui library
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_lib PUBLIC
    glfw
    OpenGL::GL
)

# Lightmap executable
add_executable(lightmap
    src/main.cpp
    src/renderer/renderer.cpp
    src/scene/scene.cpp

    include/math/vec3.h
    include/geometry/ray.h
    include/geometry/triangle.h
    include/lighting/light.h
    include/renderer/renderer.h
    include/scene/scene.h
    
)

target_include_directories(lightmap PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(lightmap PRIVATE
    glfw
    OpenGL::GL
    imgui_lib
)
