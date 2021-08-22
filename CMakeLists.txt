cmake_minimum_required(VERSION 3.14)

project(pipeline LANGUAGES CXX C)

# We want C++11
set (CMAKE_CXX_STANDARD 11)

# We need GLFW
# See: https://www.glfw.org/docs/3.3/build.html#build_link_cmake_package
# find_package(glfw3 3.2 REQUIRED)

# GLFW is non-trivial to install on Windows.
## There's not a great system package manager for Windows.
## Let's use a CMake package manager.
# The CPM dependency manager: https://github.com/TheLartians/CPM.cmake
# The Hunter package manager: https://github.com/cpp-pm/hunter

include(cmake/CPM.cmake)
CPMFindPackage(
    NAME glfw3
    # GITHUB_REPOSITORY glfw/glfw
    # GIT_TAG 3.3.2
    URL https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.zip
    URL_HASH SHA256=08a33a512f29d7dbf78eab39bd7858576adcc95228c9efe8e4bc5f0f3261efc7
    VERSION 3.2
    OPTIONS
        "ENKITS_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_DOCS OFF"
        "GLFW_BUILD_INSTALL OFF"
        )
CPMAddPackage("https://github.com/g-truc/glm.git#0.9.9.8")

# Some platforms need OpenGL. Most don't, because we are using the gl3w loader library.
find_package(OpenGL REQUIRED)

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
    src/pythonlike.h
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
target_link_libraries(pipeline glfw glm::glm ${CMAKE_DL_LIBS})

## We don't want to include the OpenGL directories because we are using gl3w.
# target_include_directories(pipeline ${OPENGL_INCLUDE_DIRS})
## On some platforms we still need to link directly.
target_link_libraries(pipeline OpenGL::GL)

# Create the screenshots directory and remember its location.
set(SCREENSHOTS "${PROJECT_SOURCE_DIR}/screenshots")
file(MAKE_DIRECTORY "${SCREENSHOTS}")
# Remember the examples directory, too.
set(EXAMPLES "${PROJECT_SOURCE_DIR}/examples")

# A CMake target to create the desired outputs
add_custom_target(screenshots
    COMMAND pipeline --screenshot "${SCREENSHOTS}/phong_bunny.png" "${EXAMPLES}/phong_bunny.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/phong_sphere.png" "${EXAMPLES}/phong_sphere.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/cel_bunny.png" "${EXAMPLES}/cel_bunny.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/cel_bunny.png" "${EXAMPLES}/matcap_bunny.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/cel_bunny.png" "${EXAMPLES}/matcap_head.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/cel_bunny.png" "${EXAMPLES}/matcap_hercules.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/cel_bunny.png" "${EXAMPLES}/matcap_lemon.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/cel_bunny.png" "${EXAMPLES}/matcap_sphere.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/normalmap_cube.png" "${EXAMPLES}/normalmap_cube.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/normalmap_head.png" "${EXAMPLES}/normalmap_head.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/normalmap_lemon.png" "${EXAMPLES}/normalmap_lemon.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/normalmap_hercules_bronze.png" "${EXAMPLES}/normalmap_hercules_bronze.json"
    COMMAND pipeline --screenshot "${SCREENSHOTS}/normalmap_hercules_marble.png" "${EXAMPLES}/normalmap_hercules_marble.json"
    
    WORKING_DIRECTORY "${OUTPUT}"
    VERBATIM
    )
# This output target depends on the meshes target having been created.
add_dependencies(screenshots pipeline)

include("CMakeLists-zip.txt" OPTIONAL)
