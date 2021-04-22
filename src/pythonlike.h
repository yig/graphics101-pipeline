#ifndef __pythonlike_h__
#define __pythonlike_h__

/*
Author: Yotam Gingold <yotam (strudel) yotamgingold.com>
License: Public Domain [CC0](http://creativecommons.org/publicdomain/zero/1.0/)
Adapted from my old "stl.h"
On GitHub as a gist: https://gist.github.com/yig/32fe51874f3911d1c612
*/

// Deleted everything but argument parsing.

#include <vector>
#include <algorithm> // find
#include <string>

namespace pythonlike
{

// Finds and removes an optional argument.
// Returns true if the argument was found.
// Returns false if the argument is not found or if the name is the last item in the vector
// (which could be considered an error).
// If the given name is not found, `value` stays unmodified. That way it can be
// initialized with the default value.
/*
// Example:
vector<string> args{ "foo", "bar", "--color", "red" };
string col = "blue"; // default
get_optional_parameter( args, "--color", col );
// Now `col` is "red" and `args` is { "foo", "bar" }.
*/
/*
// Protip: Collect argv[1:] into a vector named args:
vector<string> args( argv + 1, argv + argc );
*/
inline bool get_optional_parameter( std::vector< std::string >& args, const std::string& name, std::string& value )
{
    // Look for `name`.
    auto has_name = std::find( args.begin(), args.end(), name );
    
    // If we don't find it, return.
    if( has_name == args.end() ) return false;
    
    // It's an error to find `name` as the last argument,
    // because it must be followed by the value.
    if( has_name + 1 == args.end() ) return false;
    
    // Grab the following parameter as the setting_path.
    value = *( has_name + 1 );
    
    // Remove them from the arguments.
    args.erase( has_name, has_name + 2 );
    
    return true;
}
// Finds and removes an optional flag.
// Returns true if `name` is found in `args`, false otherwise.
/*
// Example:
vector<string> args{ "foo", "bar", "--clobber" };
bool clobber = get_optional_parameter( args, "--clobber" );
// Now `args` is { "foo", "bar" }.
*/
inline bool get_optional_parameter( std::vector< std::string >& args, const std::string& name )
{
    // Look for `name`.
    auto has_name = std::find( args.begin(), args.end(), name );
    
    // If we don't find it, return false.
    if( has_name == args.end() ) return false;
    
    // Remove it from the arguments.
    args.erase( has_name );
    
    return true;
}

}

#endif /* __pythonlike_h__ */
