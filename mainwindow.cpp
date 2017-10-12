#include <QtWidgets>

#include "mainwindow.h"
#include "glgui.h"

MainWindow::MainWindow( const std::string& scene_path )
{
    glGUI = new graphics101::GLGUI( scene_path );
    setCentralWidget( glGUI );

    createActions();
    createMenus();

    setWindowTitle( "Shader Playground" );
    resize( 500, 500 );
}

void MainWindow::open()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open JSON File...",
        QDir::currentPath(),
        "Images (*.js *.json)"
        );
    
    if( filePath.isEmpty() ) return;
    
    // https://stackoverflow.com/questions/4214369/how-to-convert-qstring-to-stdstring
    glGUI = new graphics101::GLGUI( filePath.toUtf8().constData() );
    setCentralWidget( glGUI );
}

void MainWindow::saveScreenshot()
{
    QString defaultPath = QDir::currentPath() + "/screenshot.png";

    QString savePath = QFileDialog::getSaveFileName(
        this,
        "Save As...",
        defaultPath,
        "Images (*.png *.jpg *.jpeg *.gif *.bmp *.ppm *.svg)"
        );

    if( savePath.isEmpty() ) { return; }
    
    glGUI->grabFramebuffer().save( savePath );
}

void MainWindow::about()
{
    QMessageBox::about( this, "About Shader Playground", "<p>An OpenGL Playground.</p>" );
}

void MainWindow::createActions()
{
    openAction = new QAction( tr("&Open JSON Scene..."), this );
    openAction->setShortcuts( QKeySequence::Open );
    connect( openAction, &QAction::triggered, this, &MainWindow::open );
    
    saveAction = new QAction( tr("&Save Screenshot..."), this );
    saveAction->setShortcuts( QKeySequence::Save );
    connect( saveAction, &QAction::triggered, this, &MainWindow::saveScreenshot );
    
    aboutAction = new QAction( tr("&About"), this );
    connect( aboutAction, &QAction::triggered, this, &MainWindow::about );
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu( tr("&File"), this );
    fileMenu->addAction( openAction );
    fileMenu->addAction( saveAction );

    helpMenu = new QMenu( tr("&Help"), this );
    helpMenu->addAction( aboutAction );

    menuBar()->addMenu( fileMenu );
    menuBar()->addMenu( helpMenu );
}
