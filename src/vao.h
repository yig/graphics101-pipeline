#ifndef __vao_h__
#define __vao_h__

#include "types.h"

#include <vector>
#include <memory> // shared_ptr

namespace graphics101 {

class VertexAndFaceArrays {
public:
    typedef std::shared_ptr< VertexAndFaceArrays > VertexAndFaceArraysPtr;
    static VertexAndFaceArraysPtr makePtr();
    
    VertexAndFaceArrays();
    ~VertexAndFaceArrays();
    
    // Draw the data.
    // Call ShaderProgram::use() on the shader you want to use before calling this.
    void draw();
    
    // To get the location parameter, either use the location property in GLSL declarations,
    // call ShaderProgram::getAttribLocation(), or call glGetAttribLocation( program, string ).
    // Call flatten_attribute() prior to passing data to these.
    void uploadAttribute( const GLfloat*, int num_vertices, GLuint location );
    void uploadAttribute( const vec2*, int num_vertices, GLuint location );
    void uploadAttribute( const vec3*, int num_vertices, GLuint location );
    void uploadAttribute( const vec4*, int num_vertices, GLuint location );
    void uploadAttribute( const GLint*, int num_vertices, GLuint location );
    void uploadAttribute( const ivec2*, int num_vertices, GLuint location );
    void uploadAttribute( const ivec3*, int num_vertices, GLuint location );
    void uploadAttribute( const ivec4*, int num_vertices, GLuint location );
    
    // Convenience version of the above taking an std::vector<>.
    void uploadAttribute( const std::vector< GLfloat >& attr, GLuint location ) { uploadAttribute( attr.data(), attr.size(), location ); }
    void uploadAttribute( const std::vector< vec2 >& attr, GLuint location ) { uploadAttribute( attr.data(), attr.size(), location ); }
    void uploadAttribute( const std::vector< vec3 >& attr, GLuint location ) { uploadAttribute( attr.data(), attr.size(), location ); }
    void uploadAttribute( const std::vector< vec4 >& attr, GLuint location ) { uploadAttribute( attr.data(), attr.size(), location ); }
    void uploadAttribute( const std::vector< GLint >& attr, GLuint location ) { uploadAttribute( attr.data(), attr.size(), location ); }
    void uploadAttribute( const std::vector< ivec2 >& attr, GLuint location ) { uploadAttribute( attr.data(), attr.size(), location ); }
    void uploadAttribute( const std::vector< ivec3 >& attr, GLuint location ) { uploadAttribute( attr.data(), attr.size(), location ); }
    void uploadAttribute( const std::vector< ivec4 >& attr, GLuint location ) { uploadAttribute( attr.data(), attr.size(), location ); }
    
    // Call flatten_face_indices() prior to this.
    // The default mode is GL_TRIANGLES.
    void uploadFaces( const std::vector< ivec3 >& face_indices );
    void uploadFaces( const std::vector< ivec3 >& face_indices, GLenum mode );

    // A version of uploadFaces in case your data is not stored in a vector<>.
    void uploadFaces( const GLint* face_indices, int num_face_indices );
    void uploadFaces( const GLint* face_indices, int num_face_indices, GLenum mode );
    
    // This class cannot be copied.
    VertexAndFaceArrays( const VertexAndFaceArrays& ) = delete;
    void operator=( const VertexAndFaceArrays& ) = delete;
    
private:
    GLuint m_VAO = 0;
    GLuint m_num_face_indices = 0;
    GLenum m_mode;
};

/*
Given:
    F: an #faces-by-3 sequence of faces where each row contains the
       three indices into `attribute` for the three vertices of `F`
    attributes: a sequence of vertex attributes
Returns:
    flat_attribute_out: the flattened attributes will be stored.

Fills the output variable `flat_attribute_out` such that
each entry is the attribute corresponding to the flattened vertices of `F`.
The flattened data order is [ face0_vertex0, face0_vertex1, face0_vertex2, face1_vertex0, ... ].
*/
template< typename T >
std::vector< T > flatten_attribute(
	const std::vector< glm::ivec3 >& F,
	const std::vector< T >& attribute
	)
{
	// The output flattened attributes have 3 times the number of faces.
	std::vector< T > flat_attribute_out;
	flat_attribute_out.resize( 3*F.size() );
	
	// Iterate over every vertex of every triangle.
	// Output the attributes for each one.
	int count = 0;
	for( int face_index = 0; face_index < F.size(); ++face_index ) {
		for( int face_vertex_index = 0; face_vertex_index < 3; ++face_vertex_index ) {
			// The vertex for this triangle coordinate is:
			const int attribute_index = F[ face_index ][ face_vertex_index ];
			
			// Push the attribute to the end of the output (attribute_out).
			flat_attribute_out[ count ] = attribute.at( attribute_index );
			
			count += 1;
		}
	}
	
	assert( count == flat_attribute_out.size() );
	return flat_attribute_out;
}

/*
Given:
    num_faces: the number of faces (assumed to be triangles).
Returns:
    flat_faces_out: the flattened face attribute indices.

Returns `flat_faces_out` corresponding to the flat
data order described in flatten_attribute().
*/
std::vector< ivec3 > flatten_face_indices( int num_faces );

/*
Makes a VertexAndFaceArrays with position and texture coordinate attributes
for a square. The positions range from (-1,-1) to (1,1) and the corresponding
texture coordinates from (0,0) to (1,1).
*/
VertexAndFaceArrays::VertexAndFaceArraysPtr makeSquare( GLuint position_location, GLuint texcoord_location );
/*
Makes a VertexAndFaceArrays with positions for a right triangle: (0,0), (1,0), (0,1).
Possibly useful for debugging.
*/
VertexAndFaceArrays::VertexAndFaceArraysPtr makeTriangle( GLuint position_location );

}

#endif /* __vao_h__ */
