#include "simplegui.h"

#include "glsimplescene.h"

// For error messages
#include <iostream>
using std::cerr;

namespace graphics101 {

SimpleGUI::SimpleGUI( const std::string& scene_path )
{
    m_scene = std::unique_ptr< GLSimpleScene >( new GLSimpleScene( scene_path ) );
    
    
    // If specified, the GLScene will be called periodically with a timerEvent().
    // The parameter it is passed is the number of seconds (as a real number) since
    // the class was created.
    m_creation = std::chrono::steady_clock::now();
    // 60 fps
    startTimer( 16 );
}

SimpleGUI::~SimpleGUI() {
    // Because of the unique_ptr<>, we must have a destructor, even an empty one,
    // or else the compiler complains.
    
    // If we weren't using a unique_ptr< GLScene >, we would call:
    //     delete m_scene;
    // here.
}

void SimpleGUI::initializeGL() {
    m_scene->initializeGL();
}

void SimpleGUI::resizeGL( int w, int h ) {
    m_scene->resizeGL( w, h );
}

void SimpleGUI::paintGL() {
    m_scene->paintGL();
}

void SimpleGUI::timerEvent( QTimerEvent* event ) {
    const std::chrono::duration<real> seconds_since_creation = std::chrono::steady_clock::now() - m_creation;
    m_scene->timerEvent( seconds_since_creation.count() );
    // Redraw.
    update();
}

void SimpleGUI::mousePressEvent( QMouseEvent* event ) {
    m_scene->mousePressEvent( event );
    // Redraw.
    update();
}
void SimpleGUI::mouseMoveEvent( QMouseEvent* event ) {
    m_scene->mouseMoveEvent( event );
    // Redraw.
    update();
}
void SimpleGUI::mouseReleaseEvent( QMouseEvent* event ) {
    m_scene->mouseReleaseEvent( event );
    // Redraw.
    update();
}

}
