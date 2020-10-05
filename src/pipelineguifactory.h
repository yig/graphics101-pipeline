#ifndef __PipelineGUIFactory_h__
#define __PipelineGUIFactory_h__

#include <string>
#include <memory> // unique_ptr

class QWidget;

namespace graphics101 {

typedef std::unique_ptr< QWidget > PipelineGUIPtr;

// This function peeks at the property "PipelineGUI" property of the JSON data
// stored in the file at "scene_path" and returns a new
// instance of the class with that name.
PipelineGUIPtr PipelineGUIFromScenePath( const std::string& scene_path );

// This function creates an instance of the class named `class_name`,
// passing `scene_path` to its constructor.
PipelineGUIPtr PipelineGUIFromClassName( const std::string& class_name, const std::string& scene_path );

}

#endif // __PipelineGUIFactory_h__
