QT += widgets opengl

win32 {
    LIBS += -lOpenGL32
}

linux {
    LIBS += -ldl
}

HEADERS       = \
    mainwindow.h \
    camera.h \
    debugging.h \
    drawable.h \
    filewatcher.h \
    filewatcherqt.h \
    glcompat.h \
    glfwd.h \
    glgui.h \
    glscene.h \
    glsimplescene.h \
    mesh.h \
    parsing.h \
    pipelineguifactory.h \
    shaderprogram.h \
    simplegui.h \
    texture.h \
    types.h \
    vao.h \
    stb_image.h
SOURCES       = \
    main.cpp \
    gl3w.c \
    camera.cpp \
    drawable.cpp \
    filewatcherqt.cpp \
    glgui.cpp \
    glscene.cpp \
    glsimplescene.cpp \
    mainwindow.cpp \
    mesh.cpp \
    parsing.cpp \
    pipelineguifactory.cpp \
    shaderprogram.cpp \
    simplegui.cpp \
    texture.cpp \
    vao.cpp
