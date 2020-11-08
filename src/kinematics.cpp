#include "kinematics.h"

using namespace glm;
using namespace graphics101;

namespace graphics101 {

MatrixPose forward_kinematics( const Skeleton& skeleton, const MatrixPose& bone2parent ) {
    // Verify that parents always have smaller indices than children.
    for( int i = 0; i < skeleton.size(); ++i ) { assert( skeleton.at(i).parent_index < i ); }
    
    // Initialize bone2world as bone2parent.
    MatrixPose bone2world( bone2parent );
    
    // Your code goes here.
    
    /// 1. Iterate in ascending order, which guarantees the parents are visited before
    ///    the children.
    /// 2. For each bone.
    /// 2a. Get its parent transform P and its bone-to-parent transform B.
    /// 2b. Set its bone-to-world transform to PB.
    
    return bone2world;
}

}
