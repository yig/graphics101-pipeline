# We need GLFW
find_package(glfw3 3.3 REQUIRED)

add_executable(pipeline
    camera.cpp
    drawable.cpp
    filewatchermtime.cpp
    gl3w.c
    mesh.cpp
    parsing.cpp
    shaderprogram.cpp
    texture.cpp
    vao.cpp
    
    glgui.cpp
    glscene.cpp
    glsimplescene.cpp
    main.cpp
    mainwindow.cpp
    simplegui.cpp
    pipelineguifactory.cpp

)

target_include_directories(pipeline PUBLIC ../include)
target_link_libraries(pipeline glfw)
