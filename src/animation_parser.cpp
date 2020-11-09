#include "animation.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

#include <glm/ext/matrix_transform.hpp> // translate, rotate, scale
#include <glm/gtx/matrix_interpolation.hpp> // axisAngle

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
    
    // To track the hierarchy, we need to keep track of the current parent bone.
    // We initialize the parent_index to -1 for the root.
    int parent_index = -1;
    
    // Header
    if( next( in ) != "HIERARCHY" ) {
        cerr << "ERROR: BVH file doesn't start with HIERARCHY.\n";
        return false;
    }
    
    while( true ) {
        // Read the next word.
        const string joint_or_brace = next( in );
        
        // If there is a closing brace, we should eat it and move up the hierarchy.
        if( joint_or_brace == "}" ) {
            if( parent_index < 0 ) {
                cerr << "ERROR: Closing brace found before the root node.\n";
                return false;
            }
            // Move up the hierarchy.
            parent_index = skeleton.at(parent_index).parent_index;
            // If we are back to the root, we are done.
            if( parent_index == -1 ) break;
            // Otherwise, continue;
            else continue;
        }
        
        Bone bone;
        
        // Read the node type and name.
        const string node_type = joint_or_brace;
        if( !( node_type == "ROOT" || node_type == "JOINT" || node_type == "End" ) ) {
            cerr << "ERROR: Node type expected.\n";
            return false;
        }
        
        // Read the node's name.
        in >> bone.name;
        // Set the parent index.
        bone.parent_index = parent_index;
        
        // Some error checking. The root node should be the only node
        // with a parent_index of -1.
        if( parent_index == -1 && node_type != "ROOT" ) {
            cerr << "ERROR: Top-node is not ROOT.\n";
            return false;
        } else if( parent_index >= 0 && node_type == "ROOT" ) {
            cerr << "ERROR: ROOT node appears nested.\n";
            return false;
        }
        
        // Enter the node.
        if( next( in ) != "{" ) {
            cerr << "ERROR: { doesn't follow Node declaration.\n";
            return false;
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
        if( bone.parent_index >= 0 ) bone.end += skeleton.at( bone.parent_index ).end;
        
        // Push the bone into the skeleton and store its index.
        const int bone_index = skeleton.size();
        skeleton.push_back( bone );
        // This bone is the parent of all nodes inside.
        parent_index = bone_index;
        
        // If there are channels, read them.
        if( (in >> ws).peek() == 'C' ) {
            // We need fancier parsing if this assumption fails.
            assert( next(in) == "CHANNELS" );
            
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
        skeleton: A Skeleton.
        channels: The description of the channels.
        animation: An output parameter for the animation.
    Returns:
        True if the parse was successful and false otherwise.
    
    This function calls `.clear()` on the animation.
*/
bool parseBVHAnimation( std::istream& in, const Skeleton& skeleton, const std::vector< std::pair< int, ChannelType > >& channels, BoneAnimation& animation ) {
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
    animation.frames.reserve( num_frames );
    
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
        // Initialize each transform in the pose as the identity matrix.
        MatrixPose pose( skeleton.size(), mat4(1) );
        // Set the translation to its offset from its parent's endpoint.
        for( int i = 0; i < pose.size(); ++i ) {
            const int parent_index = skeleton.at(i).parent_index;
            if( parent_index != -1 ) {
                const vec3 off = skeleton.at(i).end - skeleton.at( parent_index ).end;
                pose.at(i)[3] = vec4( off, 1 );
            }
        }
        
        for( const auto& ch : channels ) {
            real param;
            in >> param;
            if( !in ) {
                cerr << "ERROR: Could not read channel parameter.\n";
                return false;
            }
            
            // A reference, because we will modify it.
            mat4& xform = pose.at(ch.first);
            switch( ch.second ) {
                case Xposition:
                    /// Build the new operation `op`. Then multiply it on the right
                    /// of the operation so-far:
                    // pose.at(ch.first) = pose.at(ch.first)*op;
                    
                    /// Use glm functions.
                    // pose.at(ch.first) = translate( pose.at(ch.first), vec3(param,0,0) );
                    xform = glm::translate( xform, vec3(param,0,0) );
                    break;
                case Yposition:
                    xform = glm::translate( xform, vec3(0,param,0) );
                    break;
                case Zposition:
                    xform = glm::translate( xform, vec3(0,0,param) );
                    break;
                case Xrotation:
                    xform = glm::rotate( xform, glm::radians(param), vec3(1,0,0) );
                    break;
                case Yrotation:
                    xform = glm::rotate( xform, glm::radians(param), vec3(0,1,0) );
                    break;
                case Zrotation:
                    xform = glm::rotate( xform, glm::radians(param), vec3(0,0,1) );
                    break;
                default:
                    cerr << "ERROR: Unknown channel type.\n";
                    break;
            }
        }
        
        // Convert the MatrixPose to a TRSPose.
        TRSPose decomposed( skeleton.size() );
        assert( decomposed.size() == pose.size() );
        for( int i = 0; i < decomposed.size(); ++i ) {
            const mat4& M = pose.at(i);
            // Extract the translation.
            decomposed.at(i).translation = vec3(M[3]);
            // There is no scale. That means the first three columns of M should be normalized.
            assert( fabs( length(M[0]) - 1 ) < 1e-5 );
            assert( fabs( length(M[1]) - 1 ) < 1e-5 );
            assert( fabs( length(M[2]) - 1 ) < 1e-5 );
            // Extract the rotation as radians*axis.
            vec3 axis;
            real angle;
            // "If a larger matrix is constructed from a smaller matrix, the additional
            // rows and columns are set to the values they would have in an identity matrix."
            glm::axisAngle( mat4(mat3(M)), axis, angle );
            decomposed.at(i).rotation = angle*axis;
        }
        animation.frames.push_back( decomposed );
    }
    
    return true;
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
    
    if( !parseBVHAnimation( bvh, skeleton, channels, animation ) ) {
        cerr << "ERROR: Could not parse BVH animation in file: " << path << '\n';
        return false;
    }
    
    return true;
}
}
