#ifndef __kinematics_h__
#define __kinematics_h__

#include "types.h"

namespace graphics101 {

struct bone {
    // The child end point of the bone's rest pose.
    vec3 end = vec3(0,0,0);
    
    // The start point of the bone line segment is `bones[parent_index].end`.
    // The root has `parent_index` = -1. It does not have an associated line segment.
    int parent_index = -1;
    
    // The local transformation matrix associated with the bone.
    // The default value is an identity matrix (ones along the diagonal).
    mat4 transform = mat4(1);
};
typedef std::vector< bone > Skeleton;

/*
Given:
    skeleton: A Skeleton
Returns:
    A matrix for each `bone` in `skeleton` computed using forward kinematics.
*/
std::vector< mat4 > forward_kinematics( const Skeleton& skeleton );

}

#define /* __kinematics_h__ */
