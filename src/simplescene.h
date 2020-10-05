#ifndef __simplescene_h__
#define __simplescene_h__

#include "types.h"
#include "pipelineguifactory.h"

// Forward declarations.
#include "glfwd.h"
namespace graphics101 { class FileWatcher; }

#include <string>

namespace graphics101 {

class SimpleScene : public PipelineGUI
{
public:
    SimpleScene( const std::string& scene_path );
    ~SimpleScene();

    void init() override;
    void resize( int w, int h ) override;
    void draw() override;

    void mousePressEvent( const Event& event ) override;
    void mouseMoveEvent( const Event& event ) override;
    void mouseDragEvent( const Event& event ) override;
    void mouseReleaseEvent( const Event& event ) override;
    void timerEvent( real seconds_since_creation ) override;
    
private:
    void loadScene( const std::string& scene_path );
    
    // The object to draw.
    DrawablePtr m_drawable;
    
    std::string m_scene_path;
};

}

#endif /* __simplescene_h__ */
