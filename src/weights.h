#ifndef __weights_h__
#define __weights_h__

#include "types.h"
#include "kinematics.h"
namespace graphics101 { struct Mesh; }

namespace graphics101 {
/*
Given:
    point: A point in space.
    line_start, line_end: The start and end points of a line segment.
Returns:
    the distance from `point` to the line segment.
*/
real point_to_line_segment_distance( vec3 point, vec3 line_start, vec3 line_end );

/*
Given:
    point: A point in space.
    line_points: Points involved in the line segments.
    segments: Each element `e` defines a line segment. The line segment end points are
              `line_points[e[0]]` and `line_points[e[1]]`.
    k: An integer between 1 and 4, inclusive.
Returns two elements via `std::make_pair( a, b )`. Each element can be accessed via `.first` and `.second`:
    .first: An ivec4 whose elements are the indices of the closest line segments in `segments`.
    .second: A vec4 whose elements are the distances to the closest line segments in `segments`,
             in the same order.
    
    If k < 4 or there are less than 4 segments, some elements in the `ivec4`
    will be -1 and the corresponding elements in the `vec4` are undefined.
*/
std::pair< ivec4, vec4 > k_closest_distances( vec3 point, const std::vector< vec3 >& line_points, const std::vector< ivec2 >& segments, int k );

/*
Given:
    skeleton: A Skeleton.
    mesh: A Mesh.
Returns in the output parameters:
    weights_attribute: A `std::vector< vec4 >` of weights, one for each position
                       in `mesh.positions`. Each `vec4` of weights sums to 1.
    weight_sparsity_attribute: A `std::vector< ivec4 >`, one for each position
                               in `mesh.positions`. Each element of the `ivec4` specifies
                               which bone in `skeleton` the corresponding element in the
                               corresponding `vec4` in `weights_attribute`.
*/
void compute_weights(
    const Skeleton& skeleton, const Mesh& mesh,
    std::vector< vec4 >& weights_attribute,
    std::vector< ivec4 >& weight_sparsity_attribute
    );
}

#endif /* __weights_h__ */
