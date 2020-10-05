#include "pipelineguifactory.h"

// Add the headers for your pipelines here.
#include "simplescene.h"
#include "fancyscene.h"

#include "parsing.h" // loadJSONFromPath()

// For error messages
#include <iostream>
using std::cerr;

namespace graphics101 {

PipelineGUIPtr PipelineGUIFromClassName( const std::string& class_name, const std::string& scene_path ) {
    if( class_name == "SimpleScene" ) {
        return PipelineGUIPtr( new SimpleScene( scene_path ) );
    }
    else if( class_name == "FancyScene" ) {
        return PipelineGUIPtr( new FancyScene( scene_path ) );
    }
    // Add your classes here in another "else if" statement.
    else {
        cerr << "ERROR: Scene file contains an unrecognized PipelineGUI key: " << scene_path << '\n';
        return nullptr;
    }
}

PipelineGUIPtr PipelineGUIFromScenePath( const std::string& scene_path ) {
    // This function needs to peek at a single property of the JSON data
    // stored in the file at "scene_path".
    
    // Parse the file into a json object.
    json j;
    const bool success = loadJSONFromPath( scene_path, j );
    if( !success ) return nullptr;
    
    // Switch based on the PipelineGUI property of the JSON object.
    if( !j.count("PipelineGUI") ) {
        cerr << "ERROR: Scene file does not contain a PipelineGUI property: " << scene_path << '\n';
        return nullptr;
    } else {
        const std::string class_name = j["PipelineGUI"];
        return PipelineGUIFromClassName( class_name, scene_path );
    }
}

}
