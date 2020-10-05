#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

namespace graphics101 { class FancyScene; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( const std::string& scene_path );

private slots:
    void open();
    void saveScreenshot();
    void about();

private:
    void createActions();
    void createMenus();
    bool saveFile(const QByteArray &fileFormat);

    graphics101::FancyScene *glGUI;

    QMenu *fileMenu;
    QMenu *helpMenu;

    QAction* openAction;
    QAction* saveAction;
    QAction *aboutAction;
};

#endif
