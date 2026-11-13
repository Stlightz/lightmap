cmake_minimum_required(VERSION 3.20)
project(lightmap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# 1) пробуем найти уже установленный glfw (vcpkg, системный пакет, conan и т.д.)
find_package(glfw3 3.3 CONFIG QUIET)

# 2) не нашли - тянем с GitHub и собираем вместе с проектом
if(NOT glfw3_FOUND)
    message(STATUS "glfw3 не найден, скачиваю с GitHub")
    include(FetchContent)

    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
        GIT_SHALLOW    ON
    )
    FetchContent_MakeAvailable(glfw)
endif()

# Dear ImGui (скачиваем и собираем сами)
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.5
    GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(imgui)

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
target_link_libraries(imgui_lib PUBLIC glfw OpenGL::GL)

add_executable(lightmap src/main.cpp include/vec3.h include/ray.h include/triangle.h)
target_include_directories(lightmap PRIVATE include)
target_link_libraries(lightmap PRIVATE glfw OpenGL::GL imgui_lib)