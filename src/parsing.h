#ifndef __parsing_h__
#define __parsing_h__

// Some helper functions for parsing common classes from JSON.

// Forward declarations of the types to be filled.
#include "glfwd.h"
// For parsing JSON.
#include "json.hpp"
using json = nlohmann::json;

#include <string>
#include <unordered_set>
#include <functional>

namespace graphics101 {

// Given a block of JSON `j`, adds the uniforms to the UniformSet `u` and outputs the texture names in bind order.
// The incoming UniformSet `u` is not cleared first, so uniform names not in the JSON are
// left untouched.
void parseUniforms( const json& j, UniformSet& u, StringVec& texture_names_in_bind_order );

// Parses the JSON `j` to fill in a ShaderProgram `program`.
// Note that this does not/cannot delete and re-create `program`.
// Returns the set of paths accessed.
StringSet parseShader( const json& j, ShaderProgram& program );
// The same, but with an optional function parameter that will be called
// on any paths encountered prior to loading the file contents.
// The transformed paths will be returned.
typedef std::function< std::string( const std::string& ) > StringTransformer;
StringSet parseShader( const json& j, ShaderProgram& program, const StringTransformer& path_transformer );

// Loads the contents of path into the `json` object `json_out`.
// Returns true if parsing succeeded and false otherwise.
bool loadJSONFromPath( const std::string& path, json& json_out );

/*
Given:
    path: A relative path string.
    relativeToFile: A path to a file.
Returns:
    A string containing `path` relative to the file `relativeToFile`.
Examples:
    pathRelativeToFile( "bar.png", "/foo/bar.json" ) -> "/foo/bar.png"
    pathRelativeToFile( "bar.png", "C:\foo\bar.json" ) -> "C:\foo\bar.png"
    pathRelativeToFile( "bar.png", "foo/bar.json" ) -> "foo/bar.png"
    pathRelativeToFile( "bar.png", "bar.json" ) -> "./bar.png"
    pathRelativeToFile( "/bar.png", "bar.json" ) -> "./bar.png"
*/
std::string pathRelativeToFile( const std::string& path, const std::string& relativeToFile );

// Returns the contents of `path` as a string.
std::string fileAsString( const std::string& path );
}

#endif /* __parsing_h__ */
