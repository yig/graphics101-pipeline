#include "vao.h"

// Actually include the OpenGL headers.
#include "glcompat.h"

#include <glm/gtc/type_ptr.hpp> // value_ptr()

#include "mesh.h" // makeFromOBJPath, makeFromMesh

#include <iostream>

namespace {
// attribute uploading helper function
// specialized for GLfloat
void upload_attribute( GLuint VAO, GLint dimension, const GLfloat* data, int num_vertices, GLuint location )
{
    assert( num_vertices > 0 );
    assert( data );
    
    // Bind the Vertex Array Object
	glBindVertexArray( VAO );
	
    // Generate a GPU buffer
	GLuint VBO;
	glGenBuffers( 1, &VBO );
	
	// Bind the buffer
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	// Upload the data
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat)*dimension*num_vertices,
        data,
        GL_STATIC_DRAW
        );
	
	// Attach the data to the given attribute ID
	glVertexAttribPointer( location, dimension, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( location );
    
    // Unbind so we can delete the attribute buffer.
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    // Delete the attribute buffer. This won't actually delete it until we delete the VAO.
    glDeleteBuffers( 1, &VBO );
}
// specialized for GLint
void upload_attribute( GLuint VAO, GLint dimension, const GLint* data, int num_vertices, GLuint location )
{
    assert( num_vertices > 0 );
    assert( data );
    
    // Bind the Vertex Array Object
	glBindVertexArray( VAO );
	
    // Generate a GPU buffer
	GLuint VBO;
	glGenBuffers( 1, &VBO );
	
	// Bind the buffer
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	// Upload the data
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLint)*dimension*num_vertices,
        data,
        GL_STATIC_DRAW
        );
	
	// Attach the data to the given attribute ID
	glVertexAttribIPointer( location, dimension, GL_INT, 0, 0 );
    glEnableVertexAttribArray( location );
    
    // Unbind so we can delete the attribute buffer.
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    // Delete the attribute buffer. This won't actually delete it until we delete the VAO.
    glDeleteBuffers( 1, &VBO );
}

}

namespace graphics101 {

VertexAndFaceArrays::VertexAndFaceArraysPtr VertexAndFaceArrays::makePtr()
{
    return std::make_shared< VertexAndFaceArrays >();
}

VertexAndFaceArrays::VertexAndFaceArrays()
{
    glGenVertexArrays( 1, &m_VAO );
    
    assert( sizeof( vec2 ) == 2*sizeof( GLfloat ) );
    assert( sizeof( vec3 ) == 3*sizeof( GLfloat ) );
    assert( sizeof( vec4 ) == 4*sizeof( GLfloat ) );
    assert( sizeof( ivec2 ) == 2*sizeof( GLint ) );
    assert( sizeof( ivec3 ) == 3*sizeof( GLint ) );
    assert( sizeof( ivec4 ) == 4*sizeof( GLint ) );
    
    m_num_face_indices = 0;
}
VertexAndFaceArrays::~VertexAndFaceArrays()
{
    // Unbind the VAO
    glBindVertexArray( 0 );
    // Delete the VAO
    glDeleteVertexArrays( 1, &m_VAO );
    // The call to glDeleteVertexArrays() doesn't actually
    // change the value stored in m_VAO.
    // Let's set it to 0, which is an invalid value,
    // for the sake of people debugging.
    m_VAO = 0;
}

void VertexAndFaceArrays::draw()
{
    if( m_num_face_indices == 0 ) {
        std::cerr << "VertexAndFaceArray::draw() with no faces.\n";
        return;
    }
    
    glBindVertexArray( m_VAO );
    glDrawElements( m_mode, m_num_face_indices, GL_UNSIGNED_INT, 0 );
}

void VertexAndFaceArrays::uploadAttribute( const GLfloat* data, int num_vertices, GLuint location ) {
    upload_attribute( m_VAO, 1, data, num_vertices, location );
}
void VertexAndFaceArrays::uploadAttribute( const vec2* data, int num_vertices, GLuint location ) {
    upload_attribute( m_VAO, 2, num_vertices > 0 ? glm::value_ptr(data[0]) : 0, num_vertices, location );
}
void VertexAndFaceArrays::uploadAttribute( const vec3* data, int num_vertices, GLuint location ) {
    upload_attribute( m_VAO, 3, num_vertices > 0 ? glm::value_ptr(data[0]) : 0, num_vertices, location );
}
void VertexAndFaceArrays::uploadAttribute( const vec4* data, int num_vertices, GLuint location ) {
    upload_attribute( m_VAO, 4, num_vertices > 0 ? glm::value_ptr(data[0]) : 0, num_vertices, location );
}
void VertexAndFaceArrays::uploadAttribute( const GLint* data, int num_vertices, GLuint location ) {
    upload_attribute( m_VAO, 1, data, num_vertices, location );
}
void VertexAndFaceArrays::uploadAttribute( const ivec2* data, int num_vertices, GLuint location ) {
    upload_attribute( m_VAO, 2, num_vertices > 0 ? glm::value_ptr(data[0]) : 0, num_vertices, location );
}
void VertexAndFaceArrays::uploadAttribute( const ivec3* data, int num_vertices, GLuint location ) {
    upload_attribute( m_VAO, 3, num_vertices > 0 ? glm::value_ptr(data[0]) : 0, num_vertices, location );
}
void VertexAndFaceArrays::uploadAttribute( const ivec4* data, int num_vertices, GLuint location ) {
    upload_attribute( m_VAO, 4, num_vertices > 0 ? glm::value_ptr(data[0]) : 0, num_vertices, location );
}

void VertexAndFaceArrays::uploadFaces( const std::vector< ivec3 >& face_indices ) {
    uploadFaces( face_indices, GL_TRIANGLES );
}
void VertexAndFaceArrays::uploadFaces( const std::vector< ivec3 >& face_indices, GLenum mode ) {
    assert( !face_indices.empty() );
    assert( sizeof( ivec3 ) == 3*sizeof( GLint ) );
    
    uploadFaces( glm::value_ptr( face_indices.front() ), 3*face_indices.size(), mode );
}
void VertexAndFaceArrays::uploadFaces( const GLint* face_indices, int num_face_indices ) {
    uploadFaces( face_indices, num_face_indices, GL_TRIANGLES );
}
void VertexAndFaceArrays::uploadFaces( const GLint* face_indices, int num_face_indices, GLenum mode ) {
    assert( num_face_indices > 0 );
    assert( face_indices );
    
    // Remember what kind and how many indices there are.
    m_mode = mode;
    m_num_face_indices = num_face_indices;
    
    // Bind the Vertex Array Object
	glBindVertexArray( m_VAO );
	
    // Generate a GPU buffer
	GLuint FBO;
	glGenBuffers( 1, &FBO );
	
	// Bind the buffer
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, FBO );
    // Upload the data
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                 sizeof( GLint )*num_face_indices,
                 face_indices,
                 GL_STATIC_DRAW
                 );
	
	// Unlike attributes, the last bound face data is stored with the VAO.
	// We don't need to call anything else.
	
	// Unbind the VAO so we can delete the face buffer.
    glBindVertexArray( 0 );
    
    // Delete the face buffer. This won't actually delete it until we delete the VAO.
    glDeleteBuffers( 1, &FBO );
}

std::vector< ivec3 > flatten_face_indices( int num_faces ) {
    std::vector< ivec3 > flat_faces_out;
    flat_faces_out.resize( num_faces );
    
	int count = 0;
	for( int face_index = 0; face_index < num_faces; ++face_index ) {
		for( int face_vertex_index = 0; face_vertex_index < 3; ++face_vertex_index ) {
			flat_faces_out.at( face_index )[ face_vertex_index ] = count;
			count += 1;
		}
	}
	
	return flat_faces_out;
}

namespace vao {

VertexAndFaceArrays::VertexAndFaceArraysPtr makeSquare( GLuint position_location, GLuint texcoord_location )
{
    std::vector< vec3 > V;
    V.push_back( vec3( -1, -1, 0 ) );
    V.push_back( vec3(  1, -1, 0 ) );
    V.push_back( vec3(  1,  1, 0 ) );
    V.push_back( vec3( -1,  1, 0 ) );
    
    std::vector< vec2 > TC;
    TC.push_back( vec2( 0, 0 ) );
    TC.push_back( vec2( 1, 0 ) );
    TC.push_back( vec2( 1, 1 ) );
    TC.push_back( vec2( 0, 1 ) );
    
    std::vector< ivec3 > F;
    F.push_back( ivec3( 0, 1, 2 ) );
    F.push_back( ivec3( 0, 2, 3 ) );
    
    auto vao = VertexAndFaceArrays::makePtr();
    vao->uploadAttribute( V, position_location );
    vao->uploadAttribute( TC, texcoord_location );
    vao->uploadFaces( F );
    return vao;
}
VertexAndFaceArrays::VertexAndFaceArraysPtr makeTriangle( GLuint position_location )
{
    std::vector< vec3 > V;
    V.push_back( vec3( 0, 0, 0 ) );
    V.push_back( vec3( 1, 0, 0 ) );
    V.push_back( vec3( 0, 1, 0 ) );
    
    std::vector< ivec3 > F;
    F.push_back( ivec3( 0, 1, 2 ) );
    
    auto vao = VertexAndFaceArrays::makePtr();
    vao->uploadAttribute( V, position_location );
    vao->uploadFaces( F );
    return vao;
}
VertexAndFaceArrays::VertexAndFaceArraysPtr makeFromOBJPath( const std::string& OBJpath, bool create_normals_if_needed, bool normalize, GLuint position_location, GLuint normal_location, GLuint texcoord_location ) {
    // Load the mesh from the OBJ.
    Mesh mesh;
    const bool success = mesh.loadFromOBJ( OBJpath );
    if( !success ) {
        std::cerr << "ERROR: Unable to load OBJ file: " << OBJpath << '\n';
        return nullptr;
    }
    
    // Create normals if we don't have them.
    if( create_normals_if_needed && mesh.normals.size() == 0 ) {
        mesh.computeNormals();
    }
    // Normalize the mesh to fit within the unit cube [-1,1]^3 centered at the origin.
    if( normalize ) mesh.applyTransformation( mesh.normalizingTransformation() );
    
    return makeFromMesh( mesh, position_location, normal_location, texcoord_location );
}

VertexAndFaceArrays::VertexAndFaceArraysPtr makeFromMesh( const graphics101::Mesh& mesh, GLuint position_location, GLuint normal_location, GLuint texcoord_location ) {
    // Upload the mesh to the GPU.
    VertexAndFaceArrays::VertexAndFaceArraysPtr vao = VertexAndFaceArrays::makePtr();
    auto flat_positions = flatten_attribute( mesh.face_positions, mesh.positions );
    vao->uploadAttribute( flat_positions, position_location );
    
    if( normal_location != -1 ) {
        auto flat_normals = flatten_attribute( mesh.face_normals, mesh.normals );
        vao->uploadAttribute( flat_normals, normal_location );
    }
    
    if( texcoord_location != -1 && !mesh.face_texcoords.empty() ) {
        auto flat_texcoords = flatten_attribute( mesh.face_texcoords, mesh.texcoords );
        vao->uploadAttribute( flat_texcoords, texcoord_location );
    }

    auto flat_faces = flatten_face_indices( mesh.face_positions.size() );
    vao->uploadFaces( flat_faces );

    return vao;
}

}
}
