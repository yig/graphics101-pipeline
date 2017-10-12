#ifndef __glscene_h__
#define __glscene_h__

#include "types.h"

// Forward declarations.
// This header can't directly include any OpenGL or Qt headers,
// because it gets included by Qt and anti-Qt compilation units.
class QMouseEvent;
#include "glfwd.h"
namespace graphics101 { class FileWatcher; }

#include <string>

// For parsing scene JSON files.
#include "json.hpp"
using json = nlohmann::json;
#include "parsing.h" // For StringTransformer

namespace graphics101 {

class GLScene
{
public:
    GLScene( const std::string& scene_path, FileWatcher* watcher = nullptr );
    ~GLScene();

    void initializeGL();
    void resizeGL( int w, int h );
    void paintGL();

    void mousePressEvent( QMouseEvent* event );
    void mouseMoveEvent( QMouseEvent* event );
    void mouseReleaseEvent( QMouseEvent* event );
    void timerEvent( real seconds_since_creation );
    
private:
    // Sets uViewMatrix and uNormalMatrix (the inverse transpose of its upper-left 3x3).
    // Called before drawing.
    void setCameraUniforms();
    // Sets uProjectionMatrix. Called after window dimensions change.
    void setPerspectiveMatrix();

protected:
    // Reloads data that has changed (according to the m_*_changed booleans)
    // by calling the load functions below.
    void reloadChanged();
    
    // Loads the scene stored in m_scene_path.
    // Reloads the scene from the path if called again.
    void loadScene();
    
    // Call parseShaders() first, because it other parsers assume that m_program
    // has already been created.
    void loadShaders();
    void loadMesh();
    void loadUniforms();
    void loadTextures();
    
    // Transforms a path in the JSON file to a native path that can
    // be opened with a file system call. This does two things:
    // 1) Prepends the base directory of the scene file.
    //    For example, "my_scene_files/scene.json" would prepend "my_scene_files/".
    // 2) Converts slashes `/` to the native path separator (`\` on Windows).
    std::string nativePathFromJSONPath( const std::string& path ) const;
    StringTransformer nativePathFromJSONPathTransformer() const;
    
private:
    // The object to draw.
    DrawablePtr m_drawable;
    
    // Related to loading the scene.
    FileWatcher* m_watcher = nullptr;
    std::string m_scene_path;
    json m_scene;
    bool m_scene_changed = true;
    bool m_shader_changed = true;
    bool m_mesh_changed = true;
    bool m_uniforms_changed = true;
    bool m_textures_changed = true;
    StringVec m_texture_names_in_bind_order;
    
    // Related to the camera and mouse movement.
    vec2 m_camera_rotation = vec2(0,0);
    bool m_mouse_is_down = false;
    vec2 m_mouse_last_pos = vec2(-1,-1);
};

}

#endif /* __glscene_h__ */
