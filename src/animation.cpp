#include "animation.h"

#include <glm/ext/matrix_transform.hpp> // translate, rotate, scale

using namespace glm;
using namespace graphics101;

TRS::operator mat4() const {
    // rotation is radians*axis.
    const real radians = length(rotation);
    // The axis is arbitrary if the rotation is by 0 radians.
    const vec3 axis = abs(radians) > 1e-7 ? rotation/radians : vec3(0,0,1);
    
    // return translation_matrix( translation ) * rotation_matrix( radians, axis ) * scale_matrix( scale );
    // Using glm functions, this reads from inside out, "Translate the rotation of the scale."
    return glm::scale( glm::rotate( glm::translate( mat4(1), translation ), radians, axis ), scale );
}

MatrixPose MatrixPoseFromTRSPose( const TRSPose& pose ) {
    return MatrixPose( pose.begin(), pose.end() );
}

TRSPose interpolate( const BoneAnimation& animation, real t ) {
    assert( !animation.poses.empty() );
    assert( animation.seconds_per_frame > 0 );
    
    // Convert t, whose units are seconds, into u, whose units are the fraction
    // of the way through the animation. u lies in the range [0,1].
    const real total_duration_seconds = animation.poses.size()*animation.seconds_per_frame;
    // Use floating point remainder to loop back to 0.
    const real u = std::fmod( t/total_duration_seconds, 1.0 );
    
    // Your code goes here.
    TRSPose result;
    
    // The simplest possible thing: just return the closest pose.
    result = animation.poses.at( max( 0, min( int(animation.poses.size())-1, int(lround( u*(animation.poses.size()-1) )) ) ) );
    
    return result;
}
