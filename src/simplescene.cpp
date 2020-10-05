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
VertexAndFaceArraysPtr vaoFromOBJPath( const std::string& path, const ShaderProgram& program ) {
    // Load the mesh from the OBJ.
    Mesh mesh;
    const bool success = mesh.loadFromOBJ( path );
    if( !success ) {
        cerr << "ERROR: Unable to load OBJ file: " << path << '\n';
        return nullptr;
    }

    // Create normals if we don't have them.
    if( mesh.normals.size() == 0 ) {
        mesh.computeNormals( Mesh::AngleWeighted );
    }
    // Normalize the mesh to fit within the unit cube [-1,1]^3 centered at the origin.
    mesh.applyTransformation( mesh.normalizingTransformation() );

    // Upload the mesh to the GPU.
    VertexAndFaceArraysPtr vao = VertexAndFaceArrays::makePtr();
    auto flat_positions = flatten_attribute( mesh.face_positions, mesh.positions );
    vao->uploadAttribute( flat_positions, program.getAttribLocation( "vPos" ) );

    auto flat_normals = flatten_attribute( mesh.face_normals, mesh.normals );
    vao->uploadAttribute( flat_normals, program.getAttribLocation( "vNormal" ) );

    if( !mesh.face_texcoords.empty() ) {
        auto flat_texcoords = flatten_attribute( mesh.face_texcoords, mesh.texcoords );
        vao->uploadAttribute( flat_texcoords, program.getAttribLocation( "vTexCoord" ) );
    }

    auto flat_faces = flatten_face_indices( mesh.face_positions.size() );
    vao->uploadFaces( flat_faces );

    return vao;
}

std::string file_as_string( const std::string& path ) {
    // Open the file from the string path.
    std::ifstream infile( path );
    if( !infile ) {
        cerr << "ERROR: Unable to access path: " << path << '\n';
        return "";
    }
    
    // Short and efficient enough for shader-sized files.
    // From: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring/2602258#2602258
    std::stringstream buffer;
    buffer << infile.rdbuf();
    return buffer.str();
}

std::string path_relative_to_scene( const std::string& scene_path, const std::string& path ) {
    return pathRelativeToFile( path, scene_path );
    // const std::string result = pathRelativeToFile( path, scene_path );
    // std::cerr << "path_relative_to_scene( " << scene_path << ", " << path << " ): " << result << '\n';
    // return result;
}

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
    m_drawable->program->addShader( GL_VERTEX_SHADER, file_as_string( path_relative_to_scene( scene_path, "simplest.vs" ) ) );
    m_drawable->program->addShader( GL_FRAGMENT_SHADER, file_as_string( path_relative_to_scene( scene_path, "simplest.fs" ) ) );
    m_drawable->program->link();
    
    // Demonstration of setting a uniform programmatically.
    // m_drawable->program->setUniform( "uColor", vec3( 1.0, 1.0, 0.0 ) );
    
    m_drawable->vao = vaoFromOBJPath( path_relative_to_scene( scene_path, "triangle.obj" ), *m_drawable->program );
    
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
