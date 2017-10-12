#ifndef __camera_h__
#define __camera_h__

#include "types.h"

namespace graphics101 {

namespace Camera {
    // Given the screen dimensions in pixels, returns a perspective matrix
    // appropriate for viewing a potentially rotated shape contained
    // in the unit square, which itself is contained in a sphere with radius sqrt(3).
    // The parameter `eye_distance` is the same one that was passed to orbiting_world_to_camera().
    mat4 perspective_matrix_for_unit_cube( int screen_width, int screen_height, real eye_distance );
    
    // Returns a viewing matrix that converts world-space coordinates
    // to camera space. The matrix is a pure rotation and translation
    // that looks at the origin from a distance of `eye_distance` with an up vector
    // of (0,1,0), rotated by the `azimuth` and `inclination` angles in radians.
    // NOTE: Because there is no scaling, the normal matrix
    // of the result is simply the upper-left 3x3 matrix.
    mat4 orbiting_world_to_camera( real eye_distance, real azimuth, real inclination );
}

}

#endif /* __camera_h__ */
