#ifndef __FileWatcherMTime_h__
#define __FileWatcherMTime_h__

#include "filewatcher.h"

#include <unordered_map>
#include <ctime> // time_t

namespace graphics101 {

class FileWatcherMTime : public FileWatcher {

public:
    FileWatcherMTime();
    
    void watchPath( const std::string& path, const Callback& callback ) override;
    void unwatchPath( const std::string& path ) override;
    void unwatchAllPaths() override;
    
    int anythingChanged( const Callback& callback ) override;
    void removeAnythingChangedCallback( int key ) override;
    void removeAllAnythingChangedCallbacks() override;
    
    void poll() override;
    
private:
    std::unordered_map< std::string, Callback > m_watchedCallbacks;
    std::unordered_map< std::string, time_t > m_watchedMTimes;
    std::unordered_map< int, Callback > m_anythingChangedCallbacks;
    int m_anythingChangedKeyCounter;
};

}

#endif /* __FileWatcherMTime_h__ */
