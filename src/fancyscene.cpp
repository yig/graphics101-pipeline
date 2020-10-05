#include "fancyscene.h"

#include "types.h"
#include <fstream>
#include <iostream>
using std::cerr;

#include "parsing.h"
#include "filewatcher.h"

#include "debugging.h"

#include "shaderprogram.h"
#include "vao.h"
#include "texture.h"
#include "mesh.h"
#include "drawable.h"
#include "camera.h"

#include "glcompat.h"

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
    
    // Normalize the mesh to fit within the unit cube [-1,1]^3 centered at the origin.
    mesh.applyTransformation( mesh.normalizingTransformation() );
    

    // Create normals if we don't have them.
    if( mesh.normals.size() == 0 ) {
        mesh.computeNormals( Mesh::AngleWeighted );
    }
    
    // Upload the mesh to the GPU.
    VertexAndFaceArraysPtr vao = VertexAndFaceArrays::makePtr();
    auto flat_positions = flatten_attribute( mesh.face_positions, mesh.positions );
    vao->uploadAttribute( flat_positions, program.getAttribLocation( "vPos" ) );

    auto flat_normals = flatten_attribute( mesh.face_normals, mesh.normals );
    vao->uploadAttribute( flat_normals, program.getAttribLocation( "vNormal" ) );
    
    if( !mesh.face_texcoords.empty() ) {
        auto flat_texcoords = flatten_attribute( mesh.face_texcoords, mesh.texcoords );
        vao->uploadAttribute( flat_texcoords, program.getAttribLocation( "vTexCoord" ) );
        
        // Create tangent frame.
        mesh.computeTangentBitangent();
        // Upload mesh.tangents and mesh.bitangents to the GPU.
        
        // Your code goes here.
        // 1 Flatten.
        // 2 Upload to "vTangent" and "vBitangent".
    }

    auto flat_faces = flatten_face_indices( mesh.face_positions.size() );
    vao->uploadFaces( flat_faces );

    return vao;
}
}

namespace graphics101 {

FancyScene::FancyScene( const std::string& scene_path, FileWatcher* watcher )
{
    m_scene_path = scene_path;
    
    m_watcher = watcher;
}
// Without this line, we can't use the forward declaration of Drawable in a unique_ptr<>.
// From: https://stackoverflow.com/questions/13414652/forward-declaration-with-unique-ptr
// From: https://stackoverflow.com/questions/6012157/is-stdunique-ptrt-required-to-know-the-full-definition-of-t
FancyScene::~FancyScene() = default;

void FancyScene::init() {
    
    // Initialize gl3w.
    if( gl3wInit() ) {
        std::cerr << "Failed to initialize OpenGL\n";
    }
    if( !gl3wIsSupported(3,3) ) {
        std::cerr << "OpenGL 3.3 is not supported\n";
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    
    // Turn on some standard OpenGL stuff.
    
    // We want the z-buffer.
    glEnable( GL_DEPTH_TEST );
    
    // Allow alpha blending.
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    this->loadScene();
}

void FancyScene::loadScene() {
    // Mark that we are no longer out-of-date with the parsed info.
    this->m_scene_changed = false;
    
    // Unwatch all previous paths. Watch the scene.
    if( m_watcher ) {
        m_watcher->unwatchAllPaths();
        m_watcher->watchPath( m_scene_path, [=]( const std::string& ) { this->m_scene_changed = true; } );
    }
    
    
    // Parse the file into a json object.
    json j;
    const bool success = loadJSONFromPath( m_scene_path, j );
    if( !success ) return;
    
    // Save the JSON for helper functions.
    m_scene = j;
    
    if( !m_drawable ) m_drawable = Drawable::makePtr();
    
    // Make a program.
    // Everyone else expects it to have been created so we can get binding locations.
    m_drawable->program = ShaderProgram::makePtr();
    
    m_shader_changed = true;
    m_mesh_changed = true;
    m_uniforms_changed = true;
    m_textures_changed = true;
    
    // Finally, set the clear color.
    glClearColor( 1.0, 0.0, 0.0, 1.0 );
    if( j.count("ClearColor" ) ) {
        auto col = j["ClearColor"];
        if( col.size() != 4 ) {
            cerr << "ERROR: Scene JSON has a ClearColor without 4 numbers: " << m_scene_path << '\n';
        } else {
            // TODO: Check whether they are valid floating point numbers.
            glClearColor( col[0], col[1], col[2], col[3] );
        }
    }
}

void FancyScene::loadShaders() {
    assert( m_drawable && m_drawable->program );
    
    // Mark that we are no longer out-of-date with the parsed info.
    m_shader_changed = false;
    
    // The following code wants the scene JSON called `j`.
    const auto& j = m_scene;
    
    // Exit if no shaders.
    if( j.count("shaders") == 0 ) {
        cerr << "ERROR: Scene JSON has no shaders: " << m_scene_path << '\n';
        return;
    }
    
    // Load shaders.
    const StringSet paths_accessed = parseShader( j["shaders"], *m_drawable->program, nativePathFromJSONPathTransformer() );
    
    // Add shader paths to the file watcher.
    if( m_watcher ) {
        for( const auto& path : paths_accessed ) {
            m_watcher->watchPath( path, [=]( const std::string& ) { this->m_shader_changed = true; } );
        }
    }
}

void FancyScene::loadMesh() {
    assert( m_drawable && m_drawable->program );
    
    // Mark that we are no longer out-of-date with the parsed info.
    this->m_mesh_changed = false;
    
    // The following code wants the scene JSON called `j`.
    const auto& j = m_scene;
    
    /// Load the mesh.
    // Allocate a place to store the mesh on the GPU.
    m_drawable->vao.reset();
    // Load the mesh from the OBJ.
    if( j.count("mesh") == 0 || !j["mesh"].is_string() ) {
        cerr << "ERROR: Scene JSON has no mesh: " << m_scene_path << '\n';
        return;
    }
    
    // Pass the program so we have locations for the positions, normals, and texcoords.
    const auto meshpath = nativePathFromJSONPath( j["mesh"].get<std::string>() );
    m_drawable->vao = vaoFromOBJPath( meshpath, *m_drawable->program );
    
    // Add the mesh path to the filewatcher.
    if( m_watcher ) {
        m_watcher->watchPath( meshpath, [=]( const std::string& ) { this->m_mesh_changed = true; } );
    }
}

void FancyScene::loadUniforms() {
    assert( m_drawable && m_drawable->program );
    
    // Mark that we are no longer out-of-date with the parsed info.
    this->m_uniforms_changed = false;
    
    // The following code wants the scene JSON called `j`.
    const auto& j = m_scene;
    
    if( j.count("uniforms") == 0 ) {
	    cerr << "WARNING: Scene JSON has no uniforms: " << m_scene_path << '\n';
	    return;
	}
	
    // Clear the current uniforms.
    m_drawable->uniforms = UniformSet();
    // Parse the ones in the scene.
    // Store the texture names we need.
    const StringVec last_texture_names_in_bind_order = m_texture_names_in_bind_order;
    
    // If "uniforms" is a string, it's a path to another JSON file. Load it and watch it.
    if( j["uniforms"].is_string() ) {
        const auto uniformpath = nativePathFromJSONPath( j["uniforms"].get<std::string>() );
        json j_uniforms;
        const bool success = loadJSONFromPath( uniformpath, j_uniforms );
        if( success ) {
            parseUniforms( j_uniforms, m_drawable->uniforms, m_texture_names_in_bind_order );
        }
        
        // Add the mesh path to the filewatcher.
        if( m_watcher ) {
            m_watcher->watchPath( uniformpath, [=]( const std::string& ) { this->m_uniforms_changed = true; } );
        }
    }
    // Otherwise uniforms is JSON.
    else {
        parseUniforms( j["uniforms"], m_drawable->uniforms, m_texture_names_in_bind_order );
    }
    
    // If the texture names changed, reload textures.
    if( last_texture_names_in_bind_order != m_texture_names_in_bind_order ) m_textures_changed = true;
}

void FancyScene::loadTextures() {
    assert( m_drawable && m_drawable->program );
    
    // Mark that we are no longer out-of-date with the parsed info.
    this->m_textures_changed = false;
    
    // The following code wants the scene JSON called `j`.
    const auto& j = m_scene;
    const StringVec& texture_names_in_bind_order = m_texture_names_in_bind_order;
    
    // Only load textures that are needed.
    m_drawable->textures.clear();
    m_drawable->textures.resize( texture_names_in_bind_order.size() );
    if( j.count("textures") ) {
        auto j_textures = j["textures"];
        for( int i = 0; i < texture_names_in_bind_order.size(); ++i ) {
            const auto& name = texture_names_in_bind_order.at(i);
            
            if( !j_textures.count( name ) ) {
                cerr << "ERROR: Uniform references a texture name that isn't present: " << name << '\n';
                continue;
            }
            
            // Add the texture paths to the filewatcher.
            if( j_textures[name].is_string() ) {
                const auto fullpath = nativePathFromJSONPath( j_textures[name].get<std::string>() );
                m_drawable->textures.at(i) = Texture2D::makePtr( fullpath );
                // Add the texture path to the filewatcher.
                if( m_watcher ) m_watcher->watchPath( fullpath, [=]( const std::string& ) { this->m_textures_changed = true; } );
            }
            else if( j_textures[name].is_array() && j_textures[name].size() == 6 ) {
                StringVec fullpaths;
                fullpaths.resize(6);
                for( int i = 0; i < 6; ++i ) {
                    fullpaths.at(i) = nativePathFromJSONPath( j_textures[name][i].get<std::string>() );
                    // Add the texture path to the filewatcher.
                    if( m_watcher ) m_watcher->watchPath( fullpaths.at(i), [=]( const std::string& ) { this->m_textures_changed = true; } );
                }
                m_drawable->textures.at(i) = TextureCube::makePtr(
                    fullpaths[0], fullpaths[1],
                    fullpaths[2], fullpaths[3],
                    fullpaths[4], fullpaths[5]
                    );
            }
            else {
                cerr << "ERROR: Texture data is formatted incorrectly: " << j_textures[name] << '\n';
            }
        }
    }
}

void FancyScene::reloadChanged() {
    // loadScene() first, because it marks the others dirty.
    if( this->m_scene_changed   ) this->loadScene();
    if( this->m_shader_changed  ) this->loadShaders();
    if( this->m_mesh_changed   ) this->loadMesh();
    if( this->m_uniforms_changed ) this->loadUniforms();
    if( this->m_textures_changed ) this->loadTextures();
}

namespace {
ivec2 window_dimensions() {
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    const int w = viewport[2];
    const int h = viewport[3];
    return ivec2( w,h );
}
}

void FancyScene::resize( int w, int h ) {
    glViewport(0,0,w,h);
}
void FancyScene::setPerspectiveMatrix() {
    const auto width_and_height = window_dimensions();
    const int w = width_and_height[0];
    const int h = width_and_height[1];
    
    // We want to set uniforms. We need a drawable.
    assert( m_drawable );
    
    // The perspective matrix projects camera coordinates to canonical device coordinates.
    const mat4 projection = Camera::perspective_matrix_for_unit_cube( w, h, 3 );
    m_drawable->uniforms.storeUniform( "uProjectionMatrix", projection );
}
void FancyScene::setCameraUniforms() {
    // We want to set uniforms. We need a drawable.
    assert( m_drawable );

    // Update camera.
    setPerspectiveMatrix();
    
    const mat4 view = Camera::orbiting_world_to_camera( 3, m_camera_rotation[0], m_camera_rotation[1] );
    m_drawable->uniforms.storeUniform( "uViewMatrix", view );
    // Because view is just a rotation, the normal matrix is the same.
    // m_drawable->uniforms.storeUniform( "uNormal", glm::inverse( glm::transpose( mat3(view) ) ) );
    m_drawable->uniforms.storeUniform( "uNormalMatrix", mat3(view) );
}

void FancyScene::draw() {
    reloadChanged();
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    setCameraUniforms();
    
    if( m_drawable ) {
        m_drawable->bind();
        m_drawable->draw();
    }
}

void FancyScene::mousePressEvent( const Event& event ) {
    m_mouse_is_down = true;
    m_mouse_last_pos = vec2( event.x, event.y );
}
void FancyScene::mouseMoveEvent( const Event& event ) {
    const auto width_and_height = window_dimensions();

    const auto mouse_pos = vec2( event.x, event.y );
    auto diff = mouse_pos - m_mouse_last_pos;
    diff *= pi / std::min( width_and_height[0], width_and_height[1] );
    
    m_camera_rotation += diff;
    // Don't rotate past the north or south pole,
    // or else we will see the object upside-down
    // and then rotating left and right will be backwards.
    m_camera_rotation.y = glm::clamp( real( m_camera_rotation.y ), real( -pi/2 ), real( pi/2 ) );
    
    m_mouse_last_pos = mouse_pos;
}
void FancyScene::mouseReleaseEvent( const Event& event ) {
    m_mouse_is_down = false;
}
void FancyScene::timerEvent( real seconds_since_creation ) {
    // Update uniforms here.
    // draw() will be called afterwards.
    m_drawable->uniforms.storeUniform( "uTime", GLfloat( seconds_since_creation ) );
}

std::string FancyScene::nativePathFromJSONPath( const std::string& path ) const {
    return pathRelativeToFile( path, m_scene_path );
}
StringTransformer FancyScene::nativePathFromJSONPathTransformer() const {
    return [=]( const std::string& path ) { return this->nativePathFromJSONPath( path ); };
}

}
