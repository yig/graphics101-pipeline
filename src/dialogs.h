#ifndef __dialogs_h__
#define __dialogs_h__

#include <string>

namespace graphics101 {
    std::string loadFileDialog( const std::string& message, const std::string& start_dir, const std::string& filter );
    std::string saveFileDialog( const std::string& message, const std::string& start_dir, const std::string& filter );
    void errorDialog( const std::string& title, const std::string& message );
}

#endif /* __dialogs_h__ */
