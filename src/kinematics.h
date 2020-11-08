#ifndef __kinematics_h__
#define __kinematics_h__

#include "types.h"

namespace graphics101 {

struct Bone {
    // The child end point of the bone's rest pose.
    vec3 end = vec3(0,0,0);
    
    // The start point of the bone line segment is `skeleton[parent_index].end`.
    // The root has `parent_index` = -1. It does not have an associated line segment.
    int parent_index = -1;
    
    // An optional name for the bone.
    std::string name;
};
// A Skeleton is a collection of Bones.
typedef std::vector< Bone > Skeleton;
/*
A MatrixPose is an array of transformations matrices, one per bone.
The matrices can transform each bone's local coordinates to
its parent's coordinate system or to world-space.
The function `forward_kinematics()` converts a bone-to-parent Pose to a bone-to-world Pose.
*/
typedef std::vector< mat4 > MatrixPose;

/*
Given:
    skeleton: A Skeleton.
    bone2parent: A Pose with transformations from each bone's local coordinates
                 to its parent's coordinates.
Returns:
    bone2world: A Pose with transformation from each bone's local coordinates
                to world-space, computed using forward kinematics.

This function assumes that the bones in the skeleton are topologically sorted,
meaning that parents always have smaller indices than children.
*/
MatrixPose forward_kinematics( const Skeleton& skeleton, const MatrixPose& bone2parent );

}

#endif /* __kinematics_h__ */
