#include "animation.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

using namespace graphics101;

namespace {
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

// Return the next word in a stream.
std::string next( std::istream& in ) { std::string result; in >> result; return result; }
// We don't need this templated complexity:
// template< typename T > inline T next( std::istream& in ) { T result; in >> result; return result; }
}

namespace {
enum ChannelType {
    Xposition,
    Yposition,
    Zposition,
    Xrotation,
    Yrotation,
    Zrotation,
    UnknownChannelType
};
ChannelType ChannelTypeFromString( const std::string& s ) {
         if( s == "Xposition" ) return Xposition;
    else if( s == "Yposition" ) return Yposition;
    else if( s == "Zposition" ) return Zposition;
    else if( s == "Xrotation" ) return Xrotation;
    else if( s == "Yrotation" ) return Yrotation;
    else if( s == "Zrotation" ) return Zrotation;
    else                        return UnknownChannelType;
}

/*
    Given:
        in: An istream
        skeleton: An output parameter for the skeleton.
        channels: An output parameter for passing to parseBVHAnimation().
    Returns:
        True if the parse was successful and false otherwise.
    
    This function does not `.clear()` the skeleton or channels so that it can (in theory)
    be called for multiple hierarchies.
*/
bool parseBVHHierarchy( std::istream& in, Skeleton& skeleton, std::vector< std::pair< int, ChannelType > >& channels ) {
    using namespace std;
    
    // To track the hierarchy.
    // The parent_index starts at -2, so that it can increase to -1 when we read the root.
    int parent_index = -2;
    // Since this can be called for multiple hierarchies, we need to keep track
    // of the offset of the root node.
    const int skeleton0 = skeleton.size();
    // To store the output for the animation.
    
    // Header
    if( next( in ) != "HIERARCHY" ) {
        cerr << "ERROR: BVH file doesn't start with HIERARCHY.\n";
        return false;
    }
    
    while( true ) {
        const string joint_or_brace = next( in );
        // If there is a closing brace, we should eat it and move up the hierarchy.
        if( joint_or_brace == "}" ) {
            parent_index -= 1;
            // If we are back to the root, we are done.
            if( parent_index < 0 ) break;
            // Otherwise, continue;
            else continue;
        }
        
        Bone bone;
        
        // Read the node type and name.
        const string node_type = next( in );
        if( !( node_type == "ROOT" || node_type == "JOINT" || node_type == "End" ) ) {
            cerr << "ERROR: Node type expected.\n";
            return false;
        }
        
        // Read the node's name.
        in >> bone.name;
        
        if( next( in ) != "{" ) {
            cerr << "ERROR: { doesn't follow Node declaration.\n";
            return false;
        }
        // We read an opening brace. Increase our nesting level.
        parent_index += 1;
        // Set the parent index.
        if( parent_index < 0 ) {
            if( node_type != "ROOT" ) {
                cerr << "ERROR: Top-node is not ROOT.\n";
                return false;
            }
            bone.parent_index = parent_index;
        } else {
            if( node_type == "ROOT" ) {
                cerr << "ERROR: ROOT node appears nested.\n";
                return false;
            }
            bone.parent_index = parent_index + skeleton0;
        }
        
        // Read the offset from the parent.
        if( next( in ) != "OFFSET" ) {
            cerr << "ERROR: OFFSET doesn't follow {.\n";
            return false;
        }
        in >> bone.end.x >> bone.end.y >> bone.end.z;
        if( !in ) {
            cerr << "ERROR: Could not read OFFSET parameters.\n";
            return false;
        }
        // Convert from an offset to an absolute position for everything but the root.
        if( parent_index >= 0 ) bone.end += skeleton.at( parent_index ).end;
        
        // Push the bone into the skeleton and store its index.
        const int bone_index = skeleton.size();
        skeleton.push_back( bone );
        
        // If there are channels, read them.
        if( next( in ) == "CHANNELS" ) {
            int num_channels;
            in >> num_channels;
            if( !in || num_channels < 0 ) {
                cerr << "ERROR: Could not read number of CHANNELS.\n";
                return false;
            }
            for( int i = 0; i < num_channels; ++i ) {
                channels.push_back( std::make_pair( bone_index, ChannelTypeFromString( next(in) ) ) );
            }
        }
    }
    return true;
}

/*
    Given:
        in: An istream
        channels: The description of the channels.
        animation: An output parameter for the animation.
    Returns:
        True if the parse was successful and false otherwise.
    
    This function calls `.clear()` on the animation.
*/
bool parseBVHAnimation( std::istream& in, const std::vector< std::pair< int, ChannelType > >& channels, BoneAnimation& animation ) {
    using namespace std;
    
    animation.clear();
    
    if( !( next( in ) == "MOTION" ) ) {
        cerr << "ERROR: Expected keyword MOTION.\n";
        return false;
    }
    
    if( !( next( in ) == "Frames:" ) ) {
        cerr << "ERROR: Expected keyword Frames:.\n";
        return false;
    }
    int num_frames(-1);
    in >> num_frames;
    if( !in || num_frames < 0 ) {
        cerr << "ERROR: Could not read number of frames.\n";
        return false;
    }
    animation.poses.reserve( num_frames );
    
    if( !( next( in ) == "Frame" && next( in ) == "Time:" ) ) {
        cerr << "ERROR: Expected keyword Frame Time:.\n";
        return false;
    }
    in >> animation.seconds_per_frame;
    if( !in ) {
        cerr << "ERROR: Could not read seconds per frame.\n";
        return false;
    }
    
    for( int i = 0; i < num_frames; ++i ) {
        MatrixPose pose;
        
        for( const auto& ch : channels ) {
            real param;
            in >> param;
            if( !in ) {
                cerr << "ERROR: Could not read channel parameter.\n";
                return false;
            }
            
            mat4 op;
            switch( ch.second ) {
                Xposition:
                    /// Build `op`. Then left or right multiply it:
                    // pose.at(ch.first) = pose.at(ch.first)*op;
                    // pose.at(ch.first) = op*pose.at(ch.first);
                    
                    /// Or use glm functions. Is this the wrong order?
                    // pose.at(ch.first) = translate( pose.at(ch.first), vec3(param,0,0) );
                    break;
                Yposition:
                    break;
                Zposition:
                    break;
                Xrotation:
                    break;
                Yrotation:
                    break;
                Zrotation:
                    break;
                default:
                    cerr << "ERROR: Unknown channel type.\n";
                    break;
            }
        }
        
        // Convert the MatrixPose to a TRSPose.
        
    }
    
    return false;
}
}

namespace graphics101 {
// BVH format: https://research.cs.wisc.edu/graphics/Courses/cs-838-1999/Jeff/BVH.html
bool loadBVH( const std::string& path, Skeleton& skeleton, BoneAnimation& animation ) {
    using namespace std;
    
    // Clear the input.
    skeleton.clear();
    animation.clear();
    
    // Open the file.
    ifstream bvh( path );
    if( !bvh ) {
        cerr << "ERROR: loadBVH() is unable to access path: " << path << '\n';
        return false;
    }
    
    // We're not going to do a line-oriented parser this time:
    /*
    int lineno = 1;
    string line;
    // getline() does not include the newline.
    do { getline( bvh >> ws, line ); lineno+=1; } while( line.empty() );
    */
    
    std::vector< std::pair< int, ChannelType > > channels;
    if( !parseBVHHierarchy( bvh, skeleton, channels ) ) {
        cerr << "ERROR: Could not parse BVH hierarchy in file: " << path << '\n';
        return false;
    }
    
    if( !parseBVHAnimation( bvh, channels, animation ) ) {
        cerr << "ERROR: Could not parse BVH animation in file: " << path << '\n';
        return false;
    }
    
    return true;
}
}
