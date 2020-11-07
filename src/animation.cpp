#include "animation.h"

#include <glm/ext/matrix_transform.hpp> // translate, rotate, scale
#include <glm/gtx/matrix_interpolation.hpp> // axisAngleMatrix

using namespace glm;
using namespace graphics101;

namespace graphics101 {

TRS::operator mat4() const {
    // rotation is radians*axis.
    const real radians = length(rotation);
    // The axis is arbitrary if the rotation is by 0 radians.
    const vec3 axis = abs(radians) > 1e-7 ? rotation/radians : vec3(0,0,1);
    
    // return translation_matrix( translation ) * rotation_matrix( radians, axis ) * scale_matrix( scale );
    // Using glm functions, this reads from inside out, "Translate the rotation of the scale."
    // A glm transform function takes a matrix paramater P and returns PQ, where Q is the
    // requested transform.
    return glm::scale( glm::rotate( glm::translate( mat4(1), translation ), radians, axis ), scale );
}

MatrixPose MatrixPoseFromTRSPose( const TRSPose& pose ) {
    return MatrixPose( pose.begin(), pose.end() );
}

namespace {
// Returns the axis*radians rotation that results from applying the axis*radians rotation
// `first` followed by the axis*radians rotation `second`.
vec3 compose_rotations( const vec3& second, const vec3& first ) {
    // This could be done more efficiently by avoid the matrix conversion.
    // See: https://math.stackexchange.com/questions/382760/composition-of-two-axis-angle-rotations
    const mat4 Mfirst = glm::axisAngleMatrix( first, length(first) );
    const mat4 Msecond = glm::axisAngleMatrix( second, length(second) );
    const mat4 composed = Msecond*Mfirst;
    vec3 result_axis;
    real result_angle;
    glm::axisAngle( composed, result_axis, result_angle );
    return result_angle*result_axis;
}
}

vec3 slerp( const vec3& a, const vec3& b, real t ) {
    // Find the rotation `d` from `a` to `b`: da = b <=> d = b a^(-1).
    // In words, invert the `a` rotation and apply the `b` rotation to get `d`.
    const vec3 d = compose_rotations( b, -a );
    // Constant linear speed interpolation from `a` to `b` is obtained by composing
    // varying amounts of `d` with `a`: (td) a.
    // When the interpolation parameter `t` is 0, we get `a`: (0d) a = a.
    // When the interpolation parameter `t` is 1, we get `b`: 1d a = b a^(-1) a = b.
    return compose_rotations( t*d, a );
}
vec3 average_rotation( const std::vector< vec3 >& rotations, const std::vector< real >& weights ) {
    assert( rotations.size() == weights.size() );
    assert( !rotations.empty() );
    
    const real eps = 1e-5;
    
    // We are following Setion 8.5 in Ethan Eade's: http://www.ethaneade.org/lie.pdf
    
    // We need the denominator for our weighted average.
    real denom = 0.;
    for( auto w : weights ) denom += w;
    assert( denom > eps );
    denom = 1./denom;
    
    // Our initial guess is any rotation.
    vec3 mean = rotations.front();
    vec3 mean_last;
    do {
        // Store the previous mean so we know if we've stopped changing.
        mean_last = mean;
        
        // We compute the weighted average in the space around our current estimate
        // for the mean. That means, convert each rotation so that if we were at the mean
        // it would then apply the rotation.
        // (1/denom) sum w_i ( rotation_i * mean_last^(-1) )
        mean = vec3(0);
        for( int i = 0; i < rotations.size(); ++i ) {
            mean += weights.at(i) * compose_rotations( rotations.at(i), -mean_last );
        }
        mean /= denom;
        
        // Convert the average in mean-space back to global space.
        // It's our new estimate.
        mean = compose_rotations( mean, mean_last );
    } while( distance( mean, mean_last ) > eps );
    
    return mean;
}

TRSPose interpolate( const BoneAnimation& animation, real t ) {
    assert( !animation.frames.empty() );
    assert( animation.seconds_per_frame > 0 );
    
    // Convert t, whose units are seconds, into u, whose units are the fraction
    // of the way through the animation. u lies in the range [0,1].
    const real total_duration_seconds = animation.frames.size()*animation.seconds_per_frame;
    // Use floating point remainder to loop back to 0.
    const real u = std::fmod( t/total_duration_seconds, 1.0 );
    
    // Your code goes here.
    TRSPose result;
    
    // The simplest possible thing: just return the closest pose.
    result = animation.frames.at( max( 0, min( int(animation.frames.size())-1, int(lround( u*(animation.frames.size()-1) )) ) ) );
    
    return result;
}

}
