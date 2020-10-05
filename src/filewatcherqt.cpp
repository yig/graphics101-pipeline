#include "filewatcherqt.h"

#include <QFileInfo>

#include <cassert>

#include <iostream>
using std::cerr;

namespace graphics101 {

FileWatcherQt::FileWatcherQt() {
    m_anythingChangedKeyCounter = 0;
    
    // Connect signals and slots for file and directory changes.
    connect( &m_qwatcher, SIGNAL(fileChanged( const QString& )), this, SLOT(pathChanged( const QString& )) );
    connect( &m_qwatcher, SIGNAL(directoryChanged( const QString& )), this, SLOT(pathChanged( const QString& )) );
}
void FileWatcherQt::watchPath( const std::string& path, const Callback& callback ) {
    // Store the callback.
    m_watchedCallbacks[ path ] = callback;
    
    // Tell Qt to watch the path.
    // It's not an error to call this multiple times with the same path.
    m_qwatcher.addPath( path.c_str() );
}
void FileWatcherQt::unwatchPath( const std::string& path ) {
    m_watchedCallbacks.erase( path );
    m_qwatcher.removePath( path.c_str() );
}
void FileWatcherQt::unwatchAllPaths() {
    for( const auto& iter : m_watchedCallbacks ) {
        m_qwatcher.removePath( iter.first.c_str() );
    }
    m_watchedCallbacks.clear();
}

int FileWatcherQt::anythingChanged( const Callback& callback ) {
    // The unique key is the current value of the counter.
    const int result = m_anythingChangedKeyCounter;
    // Nothing should be stored with that key.
    assert( m_anythingChangedCallbacks.count( result ) == 0 );
    // Update the counter for next time.
    m_anythingChangedKeyCounter += 1;
    
    // Store the callback and return the key.
    m_anythingChangedCallbacks[ result ] = callback;
    return result;
}
void FileWatcherQt::removeAnythingChangedCallback( int key ) {
    // We must have seen this key.
    assert( m_anythingChangedCallbacks.count( key ) );
    
    m_anythingChangedCallbacks.erase( m_anythingChangedCallbacks.find( key ) );
}
void FileWatcherQt::removeAllAnythingChangedCallbacks() {
    m_anythingChangedCallbacks.clear();
}

void FileWatcherQt::pathChanged( const QString& qpath ) {
    const std::string path = qpath.toStdString();
    
    assert( m_watchedCallbacks.count( path ) );
    
    // Call the specific callback.
    m_watchedCallbacks[ path ]( path );
    
    // Call the anything-changed callback.
    for( const auto& callback : m_anythingChangedCallbacks ) callback.second( path );
    
    // In case the edit deleted and re-created the file, call addPath()
    // with the same path.
    // From: https://stackoverflow.com/questions/18300376/qt-qfilesystemwatcher-signal-filechanged-gets-emited-only-once
    {
        // Check if it still exists and is a file not a directory (because
        // we use the same slot for file and directory changes).
        QFileInfo info( qpath );
        if( info.exists() && info.isFile() ) {
            m_qwatcher.addPath( qpath );
        }
    }
}

}
