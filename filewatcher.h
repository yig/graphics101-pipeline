#ifndef __filewatcher_h__
#define __filewatcher_h__

#include <string>
#include <functional>

namespace graphics101 {

class FileWatcher {
public:
    typedef std::function<void(const std::string&)> Callback;
    
    virtual ~FileWatcher() {};
    
    // Call `callback()` if the file at `path` changes.
    // Behavior is undefined if `path` is a directory.
    // Behavior is also undefined if watchPath() is called
    // with a path already being watched.
    virtual void watchPath( const std::string& path, const Callback& callback ) = 0;
    virtual void unwatchPath( const std::string& path ) = 0;
    virtual void unwatchAllPaths() = 0;
    
    // This function will be called whenever any watched path changes.
    // It will be called after the watchPath() callbacks are called.
    // Returns a unique key that can be used to remove the callback via removeAnythingChangedCallback().
    virtual int anythingChanged( const Callback& callback ) = 0;
    virtual void removeAnythingChangedCallback( int key ) = 0;
    virtual void removeAllAnythingChangedCallbacks() = 0;
};

}

#endif /* __filewatcher_h__ */
