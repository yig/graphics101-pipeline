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
    
    // Convert to a mat4 representation automatically as needed.
    // The matrix performs translation*rotation*scale.
    operator mat4() const;
};
// A Pose as an array of transformations in TRS form.
typedef std::vector< TRS > TRSPose;
// Convert from a TRSPose to a MatrixPose.
MatrixPose MatrixPoseFromTRSPose( const TRSPose& pose );

struct BoneAnimation {
    // A bone animation stores a sequence of frames. Each frame is a pose.
    // The Poses store decomposed transformations for easier interpolation.
    // The transformations are bone-to-parent, not bone-to-world.
    std::vector< TRSPose > frames;
    
    // Frames are spaced this far apart in time.
    // By default, 60 frames/second.
    real seconds_per_frame = 1./60.;
    
    void clear() { *this = BoneAnimation(); }
};
/*
Given:
    animation: A BoneAnimation
    t: A time in seconds
Returns:
    The pose obtained by interpolating the Animation's frames at time `t`,
    where each frame lasts for `animation.seconds_per_frame`.
    The returned pose is still bone-to-parent, not bone-to-world.
    To get a bone-to-world `MatrixPose` at time `t`, use:
    `forward_kinematics( skeleton, MatrixPoseFromTRSPose( interpolate( animation, t ) ) )`
*/
TRSPose interpolate( const BoneAnimation& animation, real t );

/// These two functions can be helpful when interpolating rotations.
// Interpolate from axis*radians rotation `a` to `b` according to `t` in [0,1].
vec3 slerp( const vec3& a, const vec3& b, real t );
// Weighted average of axis*radians rotations.
vec3 average_rotation( const std::vector< vec3 >& rotations, const std::vector< real >& weights );

/*
Given:
    path: A path to a BVH file
    skeleton_out: An output parameter skeleton_out in which to store the loaded Skeleton.
    animation_out: An output parameter animation_out in which to store the loaded BoneAnimation.
Returns:
    success: A boolean that is true if parsing completed successfully and false otherwise.

The Skeleton will always store parents with smaller indices than children.
This is suitable for passing to `forward_kinematics()`.
*/
bool loadBVH( const std::string& path, Skeleton& skeleton_out, BoneAnimation& animation_out );

}

#endif /* __animation_h__ */
