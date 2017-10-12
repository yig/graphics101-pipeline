#ifndef __filewatcherqt_h__
#define __filewatcherqt_h__

#include "filewatcher.h"

#include <QFileSystemWatcher>

#include <unordered_map>
#include <vector>

namespace graphics101 {

class FileWatcherQt : public QObject, public FileWatcher {
    Q_OBJECT

public:
    FileWatcherQt();
    
    void watchPath( const std::string& path, const Callback& callback ) override;
    void unwatchPath( const std::string& path ) override;
    void unwatchAllPaths() override;
    
    // This function will be called whenever any watched path changes.
    // It will be called after the watchPath() callbacks are called.
    // Returns a unique key that can be used to remove the callback via removeAnythingChangedCallback().
    int anythingChanged( const Callback& callback ) override;
    void removeAnythingChangedCallback( int key ) override;
    void removeAllAnythingChangedCallbacks() override;
    
private:
    QFileSystemWatcher m_qwatcher;
    
    std::unordered_map< std::string, Callback > m_watchedCallbacks;
    std::unordered_map< int, Callback > m_anythingChangedCallbacks;
    int m_anythingChangedKeyCounter;
    
private slots:
    void pathChanged( const QString& path );
};

}

#endif /* __filewatcherqt_h__ */
