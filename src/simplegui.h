#ifndef __simplegui_h__
#define __simplegui_h__

// Because we want to use gl*() calls directly, we are using an OpenGL function
// loader. The built-in Qt `gl` function pointer object would require us to
// access all of our function pointers from a `gl` object as in `gl->glClear()`
// rather than simply `glClear()`. That is non-standard.
// However, because we are using a different OpenGL header, we can't
// include our OpenGL headers and the <QWidget> (or a derived type) headers
// in the same file (compilation unit).
// So GLSimpleGUI will include <QWidget> but not include any OpenGL headers,
// make any OpenGL calls, or directly store any OpenGL types.
// GLSimpleGUI will call through to GLSimpleScene for that. It uses a forward declaration to
// avoid including the GLSimpleScene header.
// GLSimpleScene will include OpenGL headers, make OpenGL calls, and store OpenGL types.

#include <QOpenGLWidget>
#include <string>
#include <memory>
#include <chrono>

namespace graphics101 {

class GLSimpleScene;

class SimpleGUI : public QOpenGLWidget
{
    Q_OBJECT
    
public:
    SimpleGUI( const std::string& scene_path );
    ~SimpleGUI();
    
    void initializeGL() override;
    void resizeGL( int w, int h ) override;
    void paintGL() override;
    
private:
    // This class forwards these events to the GLSimpleScene.
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;
    void timerEvent( QTimerEvent* event ) override;
    
    // This GLSimpleScene will receive the events.
    std::unique_ptr< GLSimpleScene > m_scene;
    
    // When this object was created. The timer event is forwarded
    // to the GLSimpleScene with a parameter that is a real number containing
    // the seconds since the object was created.
    std::chrono::steady_clock::time_point m_creation;
};

}

#endif /* __simplegui_h__ */
