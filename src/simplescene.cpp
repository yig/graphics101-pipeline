#include "simplescene.h"

#include "types.h"
#include <fstream>
#include <iostream>
#include <sstream>
using std::cerr;

#include <unordered_set>

#include "debugging.h"

#include "shaderprogram.h"
#include "vao.h"
#include "texture.h"
#include "mesh.h"
#include "drawable.h"
#include "camera.h"
#include "parsing.h"

#include "glcompat.h"

// For parsing scene JSON files.
#include "json.hpp"
using json = nlohmann::json;

using namespace graphics101;

// Helper function
namespace {
}

namespace graphics101 {

SimpleScene::SimpleScene( const std::string& scene_path )
{
    m_scene_path = scene_path;
}
// Without this line, we can't use the forward declaration of Drawable in a unique_ptr<>.
// From: https://stackoverflow.com/questions/13414652/forward-declaration-with-unique-ptr
// From: https://stackoverflow.com/questions/6012157/is-stdunique-ptrt-required-to-know-the-full-definition-of-t
SimpleScene::~SimpleScene() = default;

void SimpleScene::init() {
    
    // Turn on some standard OpenGL stuff.
    
    // We want the z-buffer.
    glEnable( GL_DEPTH_TEST );
    
    // Allow alpha blending.
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    this->loadScene( m_scene_path );
}

void SimpleScene::loadScene( const std::string& scene_path ) {
    
    m_drawable = Drawable::makePtr();
    
    // Make a program.
    // Everyone else expects it to have been created so we can get binding locations.
    m_drawable->program = ShaderProgram::makePtr();
    m_drawable->program->addShader( GL_VERTEX_SHADER, fileAsString( pathRelativeToFile( "simplest.vs", scene_path ) ) );
    m_drawable->program->addShader( GL_FRAGMENT_SHADER, fileAsString( pathRelativeToFile( "simplest.fs", scene_path ) ) );
    m_drawable->program->link();
    
    // Demonstration of setting a uniform programmatically.
    // m_drawable->program->setUniform( "uColor", vec3( 1.0, 1.0, 0.0 ) );
    
    m_drawable->vao = vao::makeFromOBJPath(
        pathRelativeToFile( "triangle.obj", scene_path ),
        // Create normals if needed.
        true,
        // Don't normalize.
        false,
        m_drawable->program->getAttribLocation( "vPos" ),
        m_drawable->program->getAttribLocation( "vNormal" ),
        m_drawable->program->getAttribLocation( "vTexCoord" )
        );
    
    // Finally, set the background color.
    glClearColor( 1.0, 0.0, 0.0, 1.0 );
}

void SimpleScene::resize( int w, int h ) {
    glViewport(0,0,w,h);
}

void SimpleScene::draw() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    m_drawable->bind();
    m_drawable->draw();
}

void SimpleScene::mousePressEvent( const Event& event ) {}
void SimpleScene::mouseMoveEvent( const Event& event ) {}
void SimpleScene::mouseDragEvent( const Event& event ) {}
void SimpleScene::mouseReleaseEvent( const Event& event ) {}
void SimpleScene::timerEvent( real seconds_since_creation ) {
    // Update uniforms here.
    // draw() will be called afterwards.
    m_drawable->uniforms.storeUniform( "uTime", GLfloat( seconds_since_creation ) );
}
}
