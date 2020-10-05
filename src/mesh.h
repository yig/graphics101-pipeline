#ifndef __mesh_h__
#define __mesh_h__

#include "types.h"
#include <vector>
#include <string>

namespace graphics101 {

struct Mesh {
public:
    // Clears the mesh and loads the OBJ file at `path`.
    // Returns true upon success and false otherwise.
    bool loadFromOBJ( const std::string& path );
    // Writes the mesh data as an OBJ to `path`.
    // Returns true upon success and false otherwise.
    bool writeToOBJ( const std::string& path );
    
    enum MeshNormalStrategy {
        Unweighted,
        AngleWeighted
    };
    // Computes per-vertex normals as the average of adjacent face normals.
    void computeNormals( MeshNormalStrategy strategy = Unweighted );
    
    // Computes tangent and bitangent vectors.
    // Requires texcoords.
    void computeTangentBitangent();
    
    // Returns the matrix that, if applied to the points of the mesh,
    // translates and uniformly scales it to tightly fit within
    // the unit cube. The unit cube is the cube with axis-aligned faces
    // centered at the origin with side length 2, aka corners at (-1,-1,-1) and (1,1,1).
    mat4 normalizingTransformation() const;
    
    // Applies a transformation to all positions in the mesh.
    // Also applies the inverse transpose transformation to all normals.
    void applyTransformation( const mat4& transform );
    
    // Clears all properties.
    void clear();
    
    std::vector< vec3 > positions;
    TriangleVec face_positions;
    
    std::vector< vec3 > normals;
    TriangleVec face_normals;
    
    std::vector< vec2 > texcoords;
    TriangleVec face_texcoords;
    
    std::vector< vec3 > tangents;
    std::vector< vec3 > bitangents;
    TriangleVec face_tangents;
};

}

#endif /* __mesh_h__ */
