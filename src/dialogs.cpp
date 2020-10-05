#include "dialogs.h"

#include <iostream>

// Consider:
// https://stackoverflow.com/questions/6145910/cross-platform-native-open-save-file-dialogs
// https://github.com/AndrewBelt/osdialog
// https://github.com/mlabbe/nativefiledialog

namespace graphics101 {

std::string loadFileDialog( const std::string& message, const std::string& start_dir, const std::string& filter ) {
    std::cerr << message << '\n';
    std::cerr << "Type a " << filter << " filename to load relative to the following path: " << start_dir << '\n';
    std::string path;
    std::getline( std::cin, path );
    if( !path.empty() && path.back() == '\n' ) {
        path = path.substr( 0, path.size()-1 );
    }
    return path;
}

std::string saveFileDialog( const std::string& message, const std::string& suggested_path, const std::string& filter ) {
    std::cerr << message << '\n';
    std::cerr << "Type a " << filter << " filename to save or press return to accept the default path: " << suggested_path << '\n';
    std::string path;
    std::getline( std::cin, path );
    if( !path.empty() && path.back() == '\n' ) {
        path = path.substr( 0, path.size()-1 );
    }
    
    // If the path is empty, then the user accepts the default.
    // This means there is no way to cancel.
    if( path.empty() ) return suggested_path;
    
    return path;
}

void errorDialog( const std::string& title, const std::string& message ) {
    std::cerr << "ERROR: " << title << ": " << message << '\n';
}

}
