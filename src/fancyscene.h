#ifndef __fancyscene_h__
#define __fancyscene_h__

#include "types.h"
#include "pipelineguifactory.h"
#include "filewatchermtime.h"
#include "animation.h"
#include "kinematics_visualizer.h"

// Forward declarations.
#include "glfwd.h"

#include <string>

// For parsing scene JSON files.
#include "json.hpp"
using json = nlohmann::json;
#include "parsing.h" // For StringTransformer

namespace graphics101 {

class FancyScene : public PipelineGUI
{
public:
    FancyScene( const std::string& scene_path );
    ~FancyScene();

    void init() override;
    void resize( int w, int h ) override;
    void draw() override;

    void mousePressEvent( const Event& event ) override;
    void mouseDragEvent( const Event& event ) override;
    void timerEvent( real seconds_since_creation ) override;
    int timerCallbackMilliseconds() override;
    
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
    void loadAnimation();
    
    // Transforms a path in the JSON file to a path that can
    // be opened with a file system call by prepending
    // the base directory of the scene file. For example,
    // the scene path "my_scene_files/scene.json" prepends "my_scene_files/"
    // to paths.
    std::string relativePathFromJSONPath( const std::string& path ) const;
    StringTransformer relativePathFromJSONPathTransformer() const;
    
private:
    // The object to draw.
    DrawablePtr m_drawable;
    
    // Related to loading the scene.
    FileWatcherMTime m_watcher;
    std::string m_scene_path;
    json m_scene;
    bool m_scene_changed = true;
    bool m_shader_changed = true;
    bool m_mesh_changed = true;
    bool m_uniforms_changed = true;
    bool m_textures_changed = true;
    bool m_animation_changed = true;
    StringVec m_texture_names_in_bind_order;
    int m_timerMilliseconds = -1;
    
    // Related to animation
    Skeleton m_skeleton;
    BoneAnimation m_animation;
    KinematicsVisualizer m_skelview;
    bool m_showSkeleton = true;
    
    // Related to the camera and mouse movement.
    vec2 m_camera_rotation = vec2(0,0);
    vec2 m_mouse_last_pos = vec2(-1,-1);
};

}

#endif /* __glscene_h__ */
