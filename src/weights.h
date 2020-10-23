#ifndef __weights_h__
#define __weights_h__

namespace graphics101 {
/*
Given:
    point: A point in space
    line_start, line_end: The start and end points of a line segment
Returns:
    the distance from `point` to the line segment.
*/
real point_to_line_segment_distance( vec3 point, vec3 line_start, vec3 line_end );

/*
Given:
    point: A point in space
    line_points: Points involved in the line segments
    
Returns:
    the distance from `point` to the line segment.
*/
// Returns the distances to the k closest
std::pair< ivec4, vec4 > k_closest_distances( vec3 point, const std::vector< vec3 >& line_points, const std::vector< ivec2 >& segments );
}

#endif /* __weights_h__ */
