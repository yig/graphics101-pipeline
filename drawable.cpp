#include "drawable.h"

#include <memory> // unique_ptr

#include <iostream>
using std::cerr;

#include "shaderprogram.h"
#include "texture.h"
#include "vao.h"

namespace graphics101 {

DrawablePtr Drawable::makePtr() {
    // return std::make_unique< Drawable >();
    return std::unique_ptr<Drawable>( new Drawable() );
}

void Drawable::bind() {
    if( !program ) {
        cerr << "No program to bind.\n";
        return;
    }
    
    program->use();
    uniforms.applyUniforms( *program );
    bind_textures( textures );
}

void Drawable::draw() {
    if( !vao ) {
        cerr << "No attributes to draw.\n";
        return;
    }
    
    vao->draw();
}

}
