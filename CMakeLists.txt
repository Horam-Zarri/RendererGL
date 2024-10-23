cmake_minimum_required( VERSION 3.10 )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(RendererGL)

find_package(OpenGL REQUIRED)

set(EXTERNAL_DIR 3rdParty)

if(UNIX AND NOT APPLE)
    set (LINUX TRUE)
endif()

if(WIN32)
    set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw-3.4.bin.WIN64)
    set(GLFW_LIBS  ${GLFW_DIR}/lib-vc2022/glfw3.lib)

    set(ASSIMP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assimp)
elseif(APPLE)
    set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw-3.4.bin.MACOS)
    set(GLFW_LIBS ${GLFW_DIR}/lib-universal/libglfw.3.dylib)

    set(ASSIMP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assimp)
    set(ASSIMP_LIBS ${ASSIMP_DIR}/bin/libassimp.5.dylib)
endif()

set(GLAD_DIR ${EXTERNAL_DIR}/glad)
set(GLM_DIR ${EXTERNAL_DIR}/glm)
set(STB_DIR ${EXTERNAL_DIR}/stb)
set(IMGUI_DIR ${EXTERNAL_DIR}/imgui)

include_directories(src)
include_directories(${GLAD_DIR}/include)
include_directories(${GLFW_DIR}/include)
include_directories(${ASSIMP_DIR}/include)
include_directories(${GLM_DIR})
include_directories(${STB_DIR})
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)


# glm double dir is required for correct include namespace
add_subdirectory(3rdParty/glm/glm glm EXCLUDE_FROM_ALL)
add_subdirectory(3rdParty/assimp assimp EXCLUDE_FROM_ALL)

file (GLOB SOURCES
    src/*.cpp
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/*.cpp
    ${GLAD_DIR}/src/glad.c
)


add_executable(RendererGL ${SOURCES})

target_link_libraries(RendererGL
    ${GLFW_LIBS}
    glm::glm
    assimp
)
