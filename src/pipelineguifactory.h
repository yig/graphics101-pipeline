#ifndef __PipelineGUIFactory_h__
#define __PipelineGUIFactory_h__

#include <string>
#include <memory> // unique_ptr

class PipelineGUI {
public:
    typedef double real;
    struct Event {
        real x = -1;
        real y = -1;
    };
    
    PipelineGUI() {}
    virtual ~PipelineGUI() {}
    
    virtual void init() {}
    virtual void resize( int w, int h ) {}
    virtual void draw() {}

    virtual void mousePressEvent( const Event& event ) {}
    virtual void mouseMoveEvent( const Event& event ) {}
    virtual void mouseReleaseEvent( const Event& event ) {}
    virtual void timerEvent( real seconds_since_creation ) {}
    // Return a non-negative number to set the desired timerEvent() and draw() interval.
    virtual int timerCallbackMilliseconds() { return -1; }
};

namespace graphics101 {

typedef std::unique_ptr< PipelineGUI > PipelineGUIPtr;

// This function peeks at the property "PipelineGUI" property of the JSON data
// stored in the file at "scene_path" and returns a new
// instance of the class with that name.
PipelineGUIPtr PipelineGUIFromScenePath( const std::string& scene_path );

// This function creates an instance of the class named `class_name`,
// passing `scene_path` to its constructor.
PipelineGUIPtr PipelineGUIFromClassName( const std::string& class_name, const std::string& scene_path );

}

#endif // __PipelineGUIFactory_h__
