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
    
    /// 1. Iterate over each bone in ascending order. This guarantees we visit parents
    ///    before children. As a result, the parent transform is already bone-to-world.
    /// 2. Get the bone's bone-to-parent transform B and its parent's bone-to-world transform P.
    /// 3. Set the bone's bone-to-world transform to: PB.
    
    return bone2world;
}

}
