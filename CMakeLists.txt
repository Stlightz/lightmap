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

add_executable(lightmap src/main.cpp include/vec3.h)
target_include_directories(lightmap PRIVATE include)
target_link_libraries(lightmap PRIVATE glfw OpenGL::GL)