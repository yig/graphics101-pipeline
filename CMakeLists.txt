cmake_minimum_required(VERSION 3.18)

project(pipeline LANGUAGES CXX C)

# We want C++11
set (CMAKE_CXX_STANDARD 11)

# We need GLFW
find_package(glfw3 3.3 REQUIRED)

add_executable(pipeline
    src/camera.cpp
    src/dialogs.cpp
    src/drawable.cpp
    src/filewatchermtime.cpp
    src/gl3w.c
    src/main.cpp
    src/mesh.cpp
    src/parsing.cpp
    src/shaderprogram.cpp
    src/texture.cpp
    src/vao.cpp
    
    src/simplescene.cpp
    src/pipelineguifactory.cpp
    src/fancyscene.cpp

)

target_include_directories(pipeline PUBLIC include)
target_link_libraries(pipeline glfw ${CMAKE_DL_LIBS})
