#include "shaderprogram.h"

#include "glcompat.h"

// To limit repetitive warnings.
#include <unordered_set>

#include <iostream>
using std::cerr;

#include <glm/gtc/type_ptr.hpp> // value_ptr()

// Uniform-related local helper functions
namespace
{

// Let's make a template function for bind_uniform and specialize it.
template< typename T >
void bind_uniform( GLint location, const T& uniform );

template<> void bind_uniform<GLfloat>( GLint location, const GLfloat& value ) {
    glUniform1f( location, value );
}
template<> void bind_uniform<glm::vec2>( GLint location, const glm::vec2& value ) {
    glUniform2fv( location, 1, glm::value_ptr(value) );
}
template<> void bind_uniform<glm::vec3>( GLint location, const glm::vec3& value ) {
    glUniform3fv( location, 1, glm::value_ptr(value) );
}
template<> void bind_uniform<glm::vec4>( GLint location, const glm::vec4& value ) {
    glUniform4fv( location, 1, glm::value_ptr(value) );
}
template<> void bind_uniform<GLint>( GLint location, const GLint& value ) {
    glUniform1i( location, value );
}
template<> void bind_uniform<glm::ivec2>( GLint location, const glm::ivec2& value ) {
    glUniform2iv( location, 1, glm::value_ptr(value) );
}
template<> void bind_uniform<glm::ivec3>( GLint location, const glm::ivec3& value ) {
    glUniform3iv( location, 1, glm::value_ptr(value) );
}
template<> void bind_uniform<glm::ivec4>( GLint location, const glm::ivec4& value ) {
    glUniform4iv( location, 1, glm::value_ptr(value) );
}
template<> void bind_uniform<glm::mat2>( GLint location, const glm::mat2& value ) {
    glUniformMatrix2fv( location, 1, GL_FALSE, glm::value_ptr(value) );
}
template<> void bind_uniform<glm::mat3>( GLint location, const glm::mat3& value ) {
    glUniformMatrix3fv( location, 1, GL_FALSE, glm::value_ptr(value) );
}
template<> void bind_uniform<glm::mat4>( GLint location, const glm::mat4& value ) {
    glUniformMatrix4fv( location, 1, GL_FALSE, glm::value_ptr(value) );
}

template<> void bind_uniform<std::vector<GLfloat>>( GLint location, const std::vector<GLfloat>& value ) {
    glUniform1fv( location, value.size(), value.data() );
}
template<> void bind_uniform<std::vector<glm::vec2>>( GLint location, const std::vector<glm::vec2>& value ) {
    glUniform2fv( location, value.size(), value.empty() ? 0 : glm::value_ptr(value.front()) );
}
template<> void bind_uniform<std::vector<glm::vec3>>( GLint location, const std::vector<glm::vec3>& value ) {
    glUniform3fv( location, value.size(), value.empty() ? 0 : glm::value_ptr(value.front()) );
}
template<> void bind_uniform<std::vector<glm::vec4>>( GLint location, const std::vector<glm::vec4>& value ) {
    glUniform4fv( location, value.size(), value.empty() ? 0 : glm::value_ptr(value.front()) );
}
template<> void bind_uniform<std::vector<GLint>>( GLint location, const std::vector<GLint>& value ) {
    glUniform1iv( location, value.size(), value.data() );
}
template<> void bind_uniform<std::vector<glm::ivec2>>( GLint location, const std::vector<glm::ivec2>& value ) {
    glUniform2iv( location, value.size(), value.empty() ? 0 : glm::value_ptr(value.front()) );
}
template<> void bind_uniform<std::vector<glm::ivec3>>( GLint location, const std::vector<glm::ivec3>& value ) {
    glUniform3iv( location, value.size(), value.empty() ? 0 : glm::value_ptr(value.front()) );
}
template<> void bind_uniform<std::vector<glm::ivec4>>( GLint location, const std::vector<glm::ivec4>& value ) {
    glUniform4iv( location, value.size(), value.empty() ? 0 : glm::value_ptr(value.front()) );
}
template<> void bind_uniform<std::vector<glm::mat2>>( GLint location, const std::vector<glm::mat2>& value ) {
    glUniformMatrix2fv( location, value.size(), GL_FALSE, value.empty() ? 0 : glm::value_ptr(value.front()) );
}
template<> void bind_uniform<std::vector<glm::mat3>>( GLint location, const std::vector<glm::mat3>& value ) {
    glUniformMatrix3fv( location, value.size(), GL_FALSE, value.empty() ? 0 : glm::value_ptr(value.front()) );
}
template<> void bind_uniform<std::vector<glm::mat4>>( GLint location, const std::vector<glm::mat4>& value ) {
    glUniformMatrix4fv( location, value.size(), GL_FALSE, value.empty() ? 0 : glm::value_ptr(value.front()) );
}

GLint location( GLuint program, const std::string& name ) {
    
    GLint loc = glGetUniformLocation( program, name.c_str() );
    
    static std::unordered_set< std::string > sLocationsUnknown;
    
    if( -1 == loc && 0 == sLocationsUnknown.count( name ) ) {
        // This is not an error. It is equivalent to an unused variable warning.
        cerr << "(Not an error; will only print once) No uniform used in the shader with the name: " << name << '\n';
        
        sLocationsUnknown.insert( name );
    }
    
    return loc;
}

}

// Shader compilation-related local helper functions
namespace
{

std::string name_from_shaderType( GLenum type )
{
    if( type == GL_VERTEX_SHADER ) {
        return "Vertex shader";
    } else if( type == GL_FRAGMENT_SHADER ) {
        return "Fragment shader";
    } else if( type == GL_GEOMETRY_SHADER ) {
        return "Geometry shader";
    } else if( type == GL_TESS_CONTROL_SHADER ) {
        return "Tessellation control shader";
    } else if( type == GL_TESS_EVALUATION_SHADER ) {
        return "Tessellation evaluation shader";
    } else {
        return "Unknown";
    }
}

std::string getShaderInfoLog( GLuint shader ) {
    std::vector< GLchar > info_log;
    {
        GLint info_log_length = 0;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &info_log_length );
        info_log.resize( info_log_length );
    }
    glGetShaderInfoLog( shader, info_log.size(), nullptr, info_log.data() );
    return std::string( info_log.data() );
}
std::string getProgramInfoLog( GLuint program ) {
    std::vector< GLchar > info_log;
    {
        GLint info_log_length = 0;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &info_log_length );
        info_log.resize( info_log_length );
    }
    glGetProgramInfoLog( program, info_log.size(), nullptr, info_log.data() );
    return std::string( info_log.data() );
}

GLuint create_and_compile_shader( GLenum shaderType, const std::vector< std::string >& codes ) {
    GLuint shader = glCreateShader( shaderType );
    {
    // Make an array of char* to pass to the OpenGL function.
    std::vector< const GLchar* > strings;
    strings.reserve( codes.size() );
    for( const auto& code : codes ) {
        strings.push_back( code.c_str() );
    }
    glShaderSource( shader, codes.size(), strings.data(), 0 );
    }
    glCompileShader( shader );
    
    GLint status = 0;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
    
    if( status != GL_TRUE ) {
        cerr << name_from_shaderType( shaderType ) << ":" << '\n';
        
        cerr << '\n';
        for( int i = 0; i < codes.size(); ++i ) {
            cerr << "==== SHADER " << i << " BEGIN" << '\n';
            cerr << codes.at(i) << '\n';
            cerr << "==== SHADER " << i << " END" << '\n';
        }
        cerr << '\n';
        
        cerr << "Shader compilation error: " << '\n' << getShaderInfoLog(shader) << '\n';
    }
    
    return shader;
}
GLuint create_and_compile_shader( GLenum shaderType, const std::string& code ) {
    return create_and_compile_shader( shaderType, std::vector< std::string >{ code } );
}


}

namespace graphics101 {

ShaderProgram::ShaderProgram()
{
    m_program = glCreateProgram();
}
ShaderProgram::~ShaderProgram()
{
    // glUseProgram(0) so that we can delete the program right away.
    glUseProgram(0);
    
    // Note: It is not an error to delete 0.
    glDeleteProgram( m_program );
    
    // The call to glDeleteProgram() doesn't actually
    // change the value stored in m_program.
    // Let's set it to 0, which is an invalid value,
    // for the sake of people debugging.
    m_program = 0;
}

ShaderProgram::ShaderProgramPtr ShaderProgram::makePtr()
{
    return std::make_shared< ShaderProgram >();
}

void ShaderProgram::use() const
{
    assert( m_program != 0 );
    
    glUseProgram( m_program );
}

GLint ShaderProgram::getUniformLocation( const std::string& name ) const {
    return glGetUniformLocation( m_program, name.c_str() );
}
GLint ShaderProgram::getAttribLocation( const std::string& name ) const {
    return glGetAttribLocation( m_program, name.c_str() );
}

void ShaderProgram::addShader( GLenum shaderType, const std::string& shader_source_code ) {
    addShader( shaderType, std::vector<std::string>{ shader_source_code } );
}
void ShaderProgram::addShader( GLenum shaderType, const std::vector< std::string >& shader_source_code ) {
    // Create and compile.
    GLuint shader = create_and_compile_shader( shaderType, shader_source_code );
    // Attach it to the program.
    glAttachShader( m_program, shader );
    // Delete it.
    // Deleting the shader won't have an effect until the shader is detached.
    // We detach it after we link.
    glDeleteShader( shader );
}
bool ShaderProgram::link() {
    glLinkProgram( m_program );
    
    GLint status = 0;
    glGetProgramiv( m_program, GL_LINK_STATUS, &status );
    
    if( status != GL_TRUE ) {
        cerr << "Shader linker error: " << '\n' << getProgramInfoLog(m_program) << '\n';
    }
    
    // Now that we have linked, detach all shaders.
    std::vector< GLuint > attached_shaders;
    {
        GLint num_attached_shaders = 0;
        glGetProgramiv( m_program, GL_ATTACHED_SHADERS, &num_attached_shaders );
        attached_shaders.resize( num_attached_shaders );
    }
    glGetAttachedShaders( m_program, attached_shaders.size(), nullptr, attached_shaders.data() );
    
	// Free the shaders by detaching them. (They have already been marked for deletion.)
	for( const auto& shader : attached_shaders ) {
	    glDetachShader( m_program, shader );
	}

    return status;
}

void ShaderProgram::setUniform( const std::string& name, GLfloat value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const vec2& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const vec3& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const vec4& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const mat2& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const mat3& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const mat4& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, GLint value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const ivec2& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const ivec3& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const ivec4& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}

void ShaderProgram::setUniform( const std::string& name, const std::vector< GLfloat >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< vec2 >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< vec3 >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< vec4 >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< mat2 >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< mat3 >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< mat4 >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< GLint >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< ivec2 >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< ivec3 >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}
void ShaderProgram::setUniform( const std::string& name, const std::vector< ivec4 >& value ) {
    use();
    bind_uniform( location( m_program, name ), value );
}

void ShaderProgram::setUniformSamplers( const std::vector< std::string >& uniform_names ) {
    use();
    
    // To bind a texture to a sampler, set the texture unit
    // as the uniform (not the texture id).
    // This function assumes that the textures are bound to the texture unit
    // corresponding to the index in the vector.
    
    for( GLint index = 0; index < uniform_names.size(); ++index ) {
        bind_uniform( location( m_program, uniform_names[index] ), index );
    }
}

void UniformSet::applyUniforms( ShaderProgram& program ) {
    for( const auto& u : m_uniforms_float ) program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_vec2 )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_vec3 )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_vec4 )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_mat2 )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_mat3 )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_mat4 )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_int )   program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_ivec2 ) program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_ivec3 ) program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_ivec4 ) program.setUniform( u.first, u.second );
    
    for( const auto& u : m_uniforms_floats ) program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_vec2s )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_vec3s )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_vec4s )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_mat2s )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_mat3s )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_mat4s )  program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_ints )   program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_ivec2s ) program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_ivec3s ) program.setUniform( u.first, u.second );
    for( const auto& u : m_uniforms_ivec4s ) program.setUniform( u.first, u.second );
    
    program.setUniformSamplers( m_uniform_samplers );
}

}
