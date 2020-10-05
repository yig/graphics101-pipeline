#ifndef __drawable_h__
#define __drawable_h__

#include "glfwd.h"
#include "shaderprogram.h"

namespace graphics101 {

struct Drawable {
    typedef std::unique_ptr< Drawable > DrawablePtr;
    static DrawablePtr makePtr();
    
    ShaderProgramPtr program;
    VertexAndFaceArraysPtr vao;
    
    UniformSet uniforms;
    TextureVec textures;
    
    // Call bind() and then draw().
    // The caller can do extra things between these two calls if desired.
    void bind();
    void draw();
};

}

#endif /* __drawable_h__ */
