cmake_minimum_required(VERSION 3.2)

project(pipeline LANGUAGES CXX C)

# We want C++11
set (CMAKE_CXX_STANDARD 11)

# We need GLFW
# See: https://www.glfw.org/docs/3.3/build.html#build_link_cmake_package
find_package(glfw3 3.3 REQUIRED)

# Some platforms need OpenGL. Most don't, because we are using the gl3w loader library.
# find_package(OpenGL REQUIRED)

set(SRCS
    src/animation.cpp
    src/animation.h
    src/animation_parser.cpp
    src/camera.cpp
    src/camera.h
    src/debugging.h
    src/dialogs.cpp
    src/dialogs.h
    src/drawable.cpp
    src/drawable.h
    src/filewatcher.h
    src/filewatchermtime.cpp
    src/filewatchermtime.h
    src/gl3w.c
    src/glcompat.h
    src/glfwd.h
    src/kinematics.cpp
    src/kinematics.h
    src/kinematics_visualizer.cpp
    src/kinematics_visualizer.h
    src/main.cpp
    src/mesh.cpp
    src/mesh.h
    src/parsing.cpp
    src/parsing.h
    src/shaderprogram.cpp
    src/shaderprogram.h
    src/stb_image_write.h
    src/stb_image.h
    src/texture.cpp
    src/texture.h
    src/types.h
    src/vao.cpp
    src/vao.h
    
    src/simplescene.cpp
    src/simplescene.h
    src/fancyscene.cpp
    src/fancyscene.h
    src/pipelineguifactory.cpp
    src/pipelineguifactory.h
)
add_executable(pipeline ${SRCS})
target_include_directories(pipeline PUBLIC include)
target_link_libraries(pipeline glfw ${CMAKE_DL_LIBS})

## We don't want to include the OpenGL directories because we are using gl3w.
# target_include_directories(pipeline ${OPENGL_INCLUDE_DIRS})
## On some platforms we still need to link directly.
# target_link_libraries(pipeline OpenGL::GL)

# Create the screenshots directory and remember its location.
set(SCREENSHOTS "${PROJECT_SOURCE_DIR}/screenshots")
file(MAKE_DIRECTORY "${SCREENSHOTS}")
# Remember the examples directory, too.
set(EXAMPLES "${PROJECT_SOURCE_DIR}/examples")

include("CMakeLists-zip.txt" OPTIONAL)
