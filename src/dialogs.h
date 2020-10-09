#ifndef __dialogs_h__
#define __dialogs_h__

#include <string>

namespace graphics101 {
    // Displays `message` and shows the user the contents of `start_dir`
    // matching the glob expression `filter` by default.
    // Returns the empty string if the user cancels.
    std::string loadFileDialog( const std::string& message, const std::string& start_dir, const std::string& filter );
    // Displays `message` and shows the user `suggested_path` as the default choice.
    // Only allows the user to choose filenames matching the glob expression `filter`.
    // Returns the empty string if the user cancels.
    std::string saveFileDialog( const std::string& message, const std::string& suggested_path, const std::string& filter );
    // Displays the `title` and error `message`.
    void errorDialog( const std::string& title, const std::string& message );
}

#endif /* __dialogs_h__ */
