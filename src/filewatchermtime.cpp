#include "filewatchermtime.h"

// https://stackoverflow.com/questions/40504281/c-how-to-check-the-last-modified-time-of-a-file/40504396#40504396
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

#include <cassert>

#include <iostream>
using std::cerr;

namespace {
time_t mtime_of_path( const std::string& path ) {
    struct stat result;
    if( stat( path.c_str(), &result ) == 0 ) {
        return result.st_mtime;
    } else {
        std::cerr << "ERROR: Unable to get mtime of path: " << path << '\n';
        return -1;
    }
}
}

namespace graphics101 {

FileWatcherMTime::FileWatcherMTime() {
    m_anythingChangedKeyCounter = 0;
}
void FileWatcherMTime::watchPath( const std::string& path, const Callback& callback ) {
    cerr << "Watching path for changes: " << path << '\n';
    
    // Store the callback.
    m_watchedCallbacks[ path ] = callback;
    // Store the current mtime.
    m_watchedMTimes[ path ] = mtime_of_path( path );
}
void FileWatcherMTime::unwatchPath( const std::string& path ) {
    m_watchedCallbacks.erase( path );
    m_watchedMTimes.erase( path );
}
void FileWatcherMTime::unwatchAllPaths() {
    m_watchedCallbacks.clear();
    m_watchedMTimes.clear();
}

int FileWatcherMTime::anythingChanged( const Callback& callback ) {
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
void FileWatcherMTime::removeAnythingChangedCallback( int key ) {
    // We must have seen this key.
    assert( m_anythingChangedCallbacks.count( key ) );
    
    m_anythingChangedCallbacks.erase( m_anythingChangedCallbacks.find( key ) );
}
void FileWatcherMTime::removeAllAnythingChangedCallbacks() {
    m_anythingChangedCallbacks.clear();
}

void FileWatcherMTime::poll() {
    // Iterate over all files.
    for( const auto& path_and_callback : m_watchedCallbacks ) {
        const auto& path = path_and_callback.first;
        
        // Is the mtime newer than the last time we checked?
        const auto new_mtime = mtime_of_path( path );
        if( m_watchedMTimes[path] < new_mtime ) {
            m_watchedMTimes[path] = new_mtime;
            
            // Call the specific callback.
            m_watchedCallbacks[ path ]( path );
            
            // Call the anything-changed callback.
            for( const auto& callback : m_anythingChangedCallbacks ) callback.second( path );
        }
    }
}

}
