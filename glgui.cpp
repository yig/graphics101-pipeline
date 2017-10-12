#include "glgui.h"

#include "glscene.h"

// For parsing scene JSON files.
#include "parsing.h"

// For error messages
#include <iostream>
using std::cerr;

// For creating the menus and save dialog.
#include <QtWidgets>

namespace {
// Returns TimerMilliseconds in the JSON file.
// If TimerMilliseconds is not present or is not a number, returns -1.
int peekAtTimerMilliseconds( const std::string& scene_path ) {

    // Open the file from the scene path.
    // This really should work, because PipelineGUIFactory() already loaded the JSON.
    json j;
    const bool success = graphics101::loadJSONFromPath( scene_path, j );
    assert( success );

    if( !j.count("TimerMilliseconds") ) return -1;

    if( !j["TimerMilliseconds"].is_number() ) {
        cerr << "ERROR: TimerMilliseconds is not a number.\n";
        return -1;
    }

    return j["TimerMilliseconds"];
}

}

namespace graphics101 {

GLGUI::GLGUI( const std::string& scene_path )
{
    m_scene = std::unique_ptr< GLScene >( new GLScene( scene_path, &m_filewatcher ) );
    
    
    // If specified, the GLScene will be called periodically with a timerEvent().
    // The parameter it is passed is the number of seconds (as a real number) since
    // the class was created.
    m_creation = std::chrono::steady_clock::now();
    
    
    // To create the timer we have to peek at the TimerMilliseconds property.
    // If there is one and it is positive, GLScene's timerEvent() will be called
    // with that interval.
    // If there is one and it is zero, GLScene's timerEvent() will be called
    // "once every time there are no more window system events to process" according
    // to the Qt startTimer docs.
    // If there is one and it is negative, a warning will be printed and no timer
    // will be scheduled.
    const int TimerMilliseconds = peekAtTimerMilliseconds( scene_path );
    if( TimerMilliseconds < 0 ) {
        cerr << "WARNING: TimerMilliseconds is negative.\n";
    } else {
        std::cout << "Creating a timer every " << TimerMilliseconds << " milliseconds.\n";
        startTimer( TimerMilliseconds );
    }
    
    
    // Create menus.
    createMenus();
    
    
    // Anytime a file changes this function will be called.
    // This is necessary because the GLScene doesn't have access to update().
    // We must call it proactively after any potential change.
    m_filewatcher.anythingChanged( [=](const std::string&) { this->update(); } );
}

GLGUI::~GLGUI() {
    // Because of the unique_ptr<>, we must have a destructor, even an empty one,
    // or else the compiler complains.
    
    // If we weren't using a unique_ptr< GLScene >, we would call:
    //     delete m_scene;
    // here.
}

void GLGUI::initializeGL() {
    m_scene->initializeGL();
}

void GLGUI::resizeGL( int w, int h ) {
    m_scene->resizeGL( w, h );
}

void GLGUI::paintGL() {
    m_scene->paintGL();
}

void GLGUI::timerEvent( QTimerEvent* event ) {
    const std::chrono::duration<real> seconds_since_creation = std::chrono::steady_clock::now() - m_creation;
    m_scene->timerEvent( seconds_since_creation.count() );
    // Redraw.
    update();
}

void GLGUI::mousePressEvent( QMouseEvent* event ) {
    m_scene->mousePressEvent( event );
    // Redraw.
    update();
}
void GLGUI::mouseMoveEvent( QMouseEvent* event ) {
    m_scene->mouseMoveEvent( event );
    // Redraw.
    update();
}
void GLGUI::mouseReleaseEvent( QMouseEvent* event ) {
    m_scene->mouseReleaseEvent( event );
    // Redraw.
    update();
}

void GLGUI::createMenus() {
    // Only create menus if this QWidget is the window.
    if( !isWindow() ) return;
    
    // Also set the window title.
    setWindowTitle( "Shader Playground" );
    
    QAction* saveAction = new QAction(tr("&Save Screenshot..."), this);
    saveAction->setShortcuts( QKeySequence::Save );
    connect( saveAction, &QAction::triggered, this, &GLGUI::saveScreenshot );
    
    QMenu* fileMenu = new QMenu( tr("&File"), this );
    fileMenu->addAction( saveAction );
    
    QMenuBar* menuBar = new QMenuBar(0);
    menuBar->addMenu( fileMenu );
}
void GLGUI::saveScreenshot() {
    QString save_path = QFileDialog::getSaveFileName(
        this,
        tr("Save Screenshot As..."),
        QDir::currentPath() + "/glscene.png",
        "Images (*.png *.jpg *.jpeg *.gif *.bmp *.ppm *.svg)"
        );
    
    if( !save_path.isEmpty() ) {
        this->grabFramebuffer().save( save_path );
    }
}

}
