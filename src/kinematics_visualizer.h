#ifndef __kinematics_visualizer_h__
#define __kinematics_visualizer_h__

#include "kinematics.h"
// Forward declarations.
#include "glfwd.h"
#include <string>
#include <vector>

namespace graphics101 {

class KinematicsVisualizer {
public:
    KinematicsVisualizer();
    KinematicsVisualizer( const std::string& asset_dir, const Skeleton& skeleton );
    void reset();
    void reset( const std::string& asset_dir, const Skeleton& skeleton );
    
    // These must be called at least once before drawing.
    void setProjectionMatrix( const mat4& projection );
    void setViewMatrix( const mat4& view );
    void setPose( const MatrixPose& bone2world );
    
    void draw();
    
private:
    std::vector< int > m_draw_bone_indices;
    DrawablePtr m_drawable;
};

}

#endif /* __kinematics_visualizer_h__ */
