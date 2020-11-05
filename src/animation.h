#ifndef __animation_h__
#define __animation_h__

#include "kinematics.h"

namespace graphics101 {

/*
We can express a transformation matrix as translation*rotation*scale.
Interpolating transformation matrices is tricky.
Interpolating translations, rotations, and scales is easier.
*/
struct TRS {
    vec3 translation; // A vector to translate by.
    vec3 rotation; // An axis*radians.
    vec3 scale; // A scale in x,y,z.
    
    // Identity by default.
    TRS() : translation(0,0,0), rotation(0,0,0), scale(1,1,1) {}
    
    // Convert to a mat4 automatically as needed.
    operator mat4();
};
// A Pose as an array of transformations in TRS form.
typedef std::vector< TRS > TRSPose;
// Convert from a TRSPose to a MatrixPose.
MatrixPose MatrixPoseFromTRSPose( const TRSPose& pose ) {
    MatrixPose result; result.reserve( pose.size() );
    for( const auto& trs : pose ) result.push_back( trs );
}

struct BoneAnimation {
    // A bone animation stores a sequence of Poses.
    // The Poses stores the local 
    std::vector< TRSPose > poses;
    
    // Frames are spaced this far apart in time.
    // By default, 60 frames/second.
    real seconds_between_frames = 1./60.;
};
// Returns the pose obtained by interpolating the Animation's poses
TRSPose interpolate( const BoneAnimation& animation, real t );

}

#define /* __animation_h__ */
