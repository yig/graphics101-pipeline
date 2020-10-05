#include <QApplication>
#include <QSurfaceFormat>
#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "pipelineguifactory.h"

int main( int argc, char* argv[] )
{
    // Request OpenGL Core Profile.
    // According to the docs, we need to set the default format
    // before QApplication is created.
    // From: http://doc.qt.io/qt-5/qopenglwidget.html#details
    QSurfaceFormat format;
    format.setRenderableType( QSurfaceFormat::OpenGL );
    format.setProfile( QSurfaceFormat::CoreProfile );
    // 3.3 has a bunch of shading language improvements we want.
    format.setVersion( 3,3 );
    QSurfaceFormat::setDefaultFormat( format );
    
    // Request desktop OpenGL drivers.
    // From: http://doc.qt.io/qt-5/windows-requirements.html
    QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL, true );
    
    QApplication app(argc, argv);
    
    // MainWindow window;
    // window.show();
    
    std::string scene_path;
    if( argc == 2 ) {
        scene_path = argv[1];
    } else {
        const QString qscene_path = QFileDialog::getOpenFileName( 0, "Open Scene", QString(), "Scene JSON Files (*.js *.json)");
        // If the user cancels, length will be zero.
        if( qscene_path.length() == 0 ) {
            // If the user cancels, don't show them an error. They cancelled. Just let them quit.
            // QMessageBox::critical( 0, "Error", "You must select a JSON scene file to proceed." );
            return -1;
        }
        scene_path = qscene_path.toStdString();
    }
    
    graphics101::PipelineGUIPtr gui = graphics101::PipelineGUIFromScenePath( scene_path );
    if( !gui ) {
        QMessageBox::critical( 0, "Error", "Unable to load scene. Check console for details." );
        return -1;
    }
    
    gui->show();

    return app.exec();
}
