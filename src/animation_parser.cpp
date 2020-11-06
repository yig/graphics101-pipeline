#include "animation.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace graphics101;

namespace
{
// Converts a string to the templated type.
template< typename T >
inline T strto( const std::string& str )
// Explicitly constructing the result (result = T(); instead of result;) means that
// built-in types (int, float, etc) will default to 0, and so return that in case of error.
{ std::istringstream converter( str ); T result = T(); converter >> result; return result; }
template< typename T >
inline T strto( const std::string& str, bool& success )
// Explicitly constructing the result (result = T(); instead of result;) means that
// built-in types (int, float, etc) will default to 0, and so return that in case of error.
// Optional output parameter `success` tells the caller this explicitly.
{ std::istringstream converter( str ); T result = T(); success = bool(converter >> result); return result; }
}

// BVH format: https://research.cs.wisc.edu/graphics/Courses/cs-838-1999/Jeff/BVH.html
bool loadBVH( const std::string& path, Skeleton& skeleton, BoneAnimation& animation ) {
    using namespace std;
    
    skeleton.clear();
    animation.clear();
    
    // TODO: Error checking with a printout to std::cerr and return false.
    
    // Open the file.
    ifstream bvh( path );
    if( !bvh ) {
        cerr << "ERROR: loadBVH() is unable to access path: " << path << '\n';
        return false;
    }
    
    // For error reporting.
    int lineno = 1;
    
    // To track the hierarchy.
    int parent_index = 0;
    // TODO: Channels
    
    // Header
    {
        string line;
        getline( bvh >> ws, line ); lineno+=1;
        if( strto<std::string>( line ) != "HIERARCHY" ) {
            cerr << "BVH file doesn't start with HIERARCHY\n";
            return false;
        }
        
        getline( bvh >> ws, line ); lineno+=1;
        
    }
    
    while( !( bvh >> ws ).eof() ) {
        // Get each line.
        string line;
        getline( bvh, line );
        
        // Skip blank lines.
        if( line.empty() ) continue;
        
        istringstream linestream( line );
        
        // Get the first word.
        string first_word;
        linestream >> first_word;
    }
    
    return true;
}
