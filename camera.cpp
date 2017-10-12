#include "camera.h"

#include <glm/gtc/matrix_transform.hpp> // glm::perspective, glm::lookAt, glm::rotate
#include <cmath>

namespace graphics101 {
namespace Camera {

mat4 perspective_matrix_for_unit_cube( int screen_width, int screen_height, real eye_distance )
{
    // A unit cube that can rotate is a sphere at the origin with radius sqrt(3).
    // We can form a right triangle.
    // Its adjacent edge is eye_distance.
    // Its opposite edge is sqrt(3) padded by however much padding we want between
    // the object and the edge of the window.
    // The tangent( field-of-view angle / 2 ) = opposite/adjacent.
    // atan() gives us the angle/2.
    
    assert( eye_distance > 0.0 );
    assert( eye_distance > sqrt(3) );
    
    const real aspect = real(screen_width)/screen_height;
    
    const real padding_factor = 1.01;
    const real adjacent = eye_distance;
    real opposite = sqrt(3) * padding_factor;
    if( screen_width < screen_height ) {
        opposite = opposite / aspect;
    }
    const real fov_y = 2.0*atan( opposite / adjacent );
    
    // Multiply by 1.01 to add 1% of padding to the clipping planes.
    const real zNear = eye_distance - sqrt(3)*1.01;
    const real zFar  = eye_distance + 2*sqrt(3)*1.01;
    // const real zNear = 0.01;
    // const real zFar = 10000;
    
    return glm::perspective( fov_y, aspect, zNear, zFar );
}

mat4 orbiting_world_to_camera( real eye_distance, real azimuth, real inclination ) {
    // The view matrix converts from world to camera coordinates. It is a rotation.

    // Apply the mouse rotation.
    // First azimuth.
    // NOTE: glm::rotate() returns the first argument * rotation.
    mat4 view = glm::rotate( mat4(1), azimuth, vec3( 0,1,0 ) );
    // Then inclination.
    view = glm::rotate( mat4(1), inclination, vec3( 1,0,0 ) ) * view;
    
    // Finally convert to camera-space coordinates.
    view = glm::lookAt( vec3( 0,0,eye_distance ), vec3( 0,0,0 ), vec3( 0,1,0 ) ) * view;

    return view;
}

}
}
