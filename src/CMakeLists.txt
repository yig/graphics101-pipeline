# We need GLFW
find_package(glfw3 3.3 REQUIRED)

add_executable(pipeline
    camera.cpp
    dialogs.cpp
    drawable.cpp
    filewatchermtime.cpp
    gl3w.c
    main.cpp
    mesh.cpp
    parsing.cpp
    shaderprogram.cpp
    texture.cpp
    vao.cpp
    
    simplescene.cpp
    pipelineguifactory.cpp
    fancyscene.cpp

)

target_include_directories(pipeline PUBLIC ../include)
target_link_libraries(pipeline glfw)
