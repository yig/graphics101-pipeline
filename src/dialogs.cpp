#include "dialogs.h"

#include <iostream>

// Consider:
// https://stackoverflow.com/questions/6145910/cross-platform-native-open-save-file-dialogs
// https://github.com/AndrewBelt/osdialog
// https://github.com/mlabbe/nativefiledialog
// https://github.com/guillaumechereau/noc/blob/master/noc_file_dialog.h
// https://sourceforge.net/projects/tinyfiledialogs/

namespace graphics101 {

std::string loadFileDialog( const std::string& message, const std::string& start_dir, const std::string& filter ) {
    std::cout << message << '\n';
    std::cout << "Type a " << filter << " filename to load: ";
    std::cout.flush();
    std::string path;
    std::getline( std::cin, path );
    // getline() returns a string with the newline appended. Trim it off.
    if( !path.empty() && path.back() == '\n' ) {
        path = path.substr( 0, path.size()-1 );
    }
    return path;
}

std::string saveFileDialog( const std::string& message, const std::string& suggested_path, const std::string& filter ) {
    std::cout << message << '\n';
    std::cout << "Type a " << filter << " filename to save or press return to accept the default path: " << suggested_path << '\n';
    std::cout << "Filename: ";
    std::cout.flush();
    std::string path;
    std::getline( std::cin, path );
    // getline() returns a string with the newline appended. Trim it off.
    if( !path.empty() && path.back() == '\n' ) {
        path = path.substr( 0, path.size()-1 );
    }
    
    // If the user inputs an empty string, then path is empty.
    // We take that to mean that the user accepts the `suggested_path`.
    // This means we don't provide the user a way to cancel,
    // which is an option they usually have with a dialog box.
    if( path.empty() ) return suggested_path;
    
    return path;
}

void errorDialog( const std::string& title, const std::string& message ) {
    std::cerr << "ERROR: " << title << ": " << message << '\n';
}

}
