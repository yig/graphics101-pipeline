#ifndef __shaderprogram_h__
#define __shaderprogram_h__

#include "types.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <memory> // shared_ptr

namespace graphics101 {

class ShaderProgram {
public:
    typedef std::shared_ptr< ShaderProgram > ShaderProgramPtr;
    static ShaderProgramPtr makePtr();
    
    ShaderProgram();
    ~ShaderProgram();
    
    // Add shaders with addShader(), then call link().
    // Possible shaderTypes are:
    //      GL_VERTEX_SHADER, GL_FRAGMENT_SHADER,
    //      GL_GEOMETRY_SHADER,
    //      GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER
    void addShader( GLenum shaderType, const std::string& shader_source_code );
    void addShader( GLenum shaderType, const std::vector< std::string >& shader_source_code );
    // Returns true if linking succeeded, false otherwise.
    bool link();
    
    GLint getUniformLocation( const std::string& name ) const;
    GLint getAttribLocation( const std::string& name ) const;
    
    void use() const;

    // Called by the GLScene or whomever.
    void setUniform( const std::string& name, GLfloat value );
    void setUniform( const std::string& name, const vec2& value );
    void setUniform( const std::string& name, const vec3& value );
    void setUniform( const std::string& name, const vec4& value );
    void setUniform( const std::string& name, const mat2& value );
    void setUniform( const std::string& name, const mat3& value );
    void setUniform( const std::string& name, const mat4& value );
    void setUniform( const std::string& name, GLint value );
    void setUniform( const std::string& name, const ivec2& value );
    void setUniform( const std::string& name, const ivec3& value );
    void setUniform( const std::string& name, const ivec4& value );
    
    void setUniform( const std::string& name, const std::vector< GLfloat >& value );
    void setUniform( const std::string& name, const std::vector< vec2 >& value );
    void setUniform( const std::string& name, const std::vector< vec3 >& value );
    void setUniform( const std::string& name, const std::vector< vec4 >& value );
    void setUniform( const std::string& name, const std::vector< mat2 >& value );
    void setUniform( const std::string& name, const std::vector< mat3 >& value );
    void setUniform( const std::string& name, const std::vector< mat4 >& value );
    void setUniform( const std::string& name, const std::vector< GLint >& value );
    void setUniform( const std::string& name, const std::vector< ivec2 >& value );
    void setUniform( const std::string& name, const std::vector< ivec3 >& value );
    void setUniform( const std::string& name, const std::vector< ivec4 >& value );
    
    // To use sampler() functions, pass the list of uniform names.
    // This function assumes that the textures are bound to the texture unit
    // corresponding to the index in the vector.
    void setUniformSamplers( const std::vector< std::string >& uniform_names );
    
    // This class cannot be copied. Use a ShaderProgramPtr.
    ShaderProgram( const ShaderProgram& ) = delete;
    void operator=( const ShaderProgram& ) = delete;
    
private:
    GLuint m_program;
};

/*
A class which stores a set of uniforms for setting later.
*/
struct UniformSet {
public:
    // Apply the stored uniforms to the program.
    void applyUniforms( ShaderProgram& program );
    
    void storeUniform( const std::string& name, GLfloat value )      { m_uniforms_float[name] = value; }
    void storeUniform( const std::string& name, const vec2& value )  { m_uniforms_vec2[name] = value; }
    void storeUniform( const std::string& name, const vec3& value )  { m_uniforms_vec3[name] = value; }
    void storeUniform( const std::string& name, const vec4& value )  { m_uniforms_vec4[name] = value; }
    void storeUniform( const std::string& name, const mat2& value )  { m_uniforms_mat2[name] = value; }
    void storeUniform( const std::string& name, const mat3& value )  { m_uniforms_mat3[name] = value; }
    void storeUniform( const std::string& name, const mat4& value )  { m_uniforms_mat4[name] = value; }
    void storeUniform( const std::string& name, GLint value )        { m_uniforms_int[name] = value; }
    void storeUniform( const std::string& name, const ivec2& value ) { m_uniforms_ivec2[name] = value; }
    void storeUniform( const std::string& name, const ivec3& value ) { m_uniforms_ivec3[name] = value; }
    void storeUniform( const std::string& name, const ivec4& value ) { m_uniforms_ivec4[name] = value; }
    
    void storeUniform( const std::string& name, const std::vector< GLfloat >& value ) { m_uniforms_floats[name] = value; }
    void storeUniform( const std::string& name, const std::vector< vec2 >& value )    { m_uniforms_vec2s[name] = value; }
    void storeUniform( const std::string& name, const std::vector< vec3 >& value )    { m_uniforms_vec3s[name] = value; }
    void storeUniform( const std::string& name, const std::vector< vec4 >& value )    { m_uniforms_vec4s[name] = value; }
    void storeUniform( const std::string& name, const std::vector< mat2 >& value )    { m_uniforms_mat2s[name] = value; }
    void storeUniform( const std::string& name, const std::vector< mat3 >& value )    { m_uniforms_mat3s[name] = value; }
    void storeUniform( const std::string& name, const std::vector< mat4 >& value )    { m_uniforms_mat4s[name] = value; }
    void storeUniform( const std::string& name, const std::vector< GLint >& value )   { m_uniforms_ints[name] = value; }
    void storeUniform( const std::string& name, const std::vector< ivec2 >& value )   { m_uniforms_ivec2s[name] = value; }
    void storeUniform( const std::string& name, const std::vector< ivec3 >& value )   { m_uniforms_ivec3s[name] = value; }
    void storeUniform( const std::string& name, const std::vector< ivec4 >& value )   { m_uniforms_ivec4s[name] = value; }
    
    // To use sampler() functions, pass the uniform names.
    // This version of the function assumes that the textures are bound to texture units
    // in the order this function was called.
    void storeUniformSampler( const std::string& uniform_name )      { m_uniform_samplers.push_back( uniform_name ); }
    // This function assumes that the textures are bound to the texture unit
    // corresponding to the index in the vector.
    void storeUniformSamplers( const std::vector< std::string >& uniform_names )      { m_uniform_samplers = uniform_names; }
    
private:
    std::unordered_map< std::string, GLfloat > m_uniforms_float;
    std::unordered_map< std::string, vec2    > m_uniforms_vec2;
    std::unordered_map< std::string, vec3    > m_uniforms_vec3;
    std::unordered_map< std::string, vec4    > m_uniforms_vec4;
    std::unordered_map< std::string, mat2    > m_uniforms_mat2;
    std::unordered_map< std::string, mat3    > m_uniforms_mat3;
    std::unordered_map< std::string, mat4    > m_uniforms_mat4;
    std::unordered_map< std::string, GLint   > m_uniforms_int;
    std::unordered_map< std::string, ivec2   > m_uniforms_ivec2;
    std::unordered_map< std::string, ivec3   > m_uniforms_ivec3;
    std::unordered_map< std::string, ivec4   > m_uniforms_ivec4;
    
    std::unordered_map< std::string, std::vector< GLfloat > > m_uniforms_floats;
    std::unordered_map< std::string, std::vector< vec2    > > m_uniforms_vec2s;
    std::unordered_map< std::string, std::vector< vec3    > > m_uniforms_vec3s;
    std::unordered_map< std::string, std::vector< vec4    > > m_uniforms_vec4s;
    std::unordered_map< std::string, std::vector< mat2    > > m_uniforms_mat2s;
    std::unordered_map< std::string, std::vector< mat3    > > m_uniforms_mat3s;
    std::unordered_map< std::string, std::vector< mat4    > > m_uniforms_mat4s;
    std::unordered_map< std::string, std::vector< GLint   > > m_uniforms_ints;
    std::unordered_map< std::string, std::vector< ivec2   > > m_uniforms_ivec2s;
    std::unordered_map< std::string, std::vector< ivec3   > > m_uniforms_ivec3s;
    std::unordered_map< std::string, std::vector< ivec4   > > m_uniforms_ivec4s;
    
    std::vector< std::string > m_uniform_samplers;
};

}

#endif /* __shaderprogram_h__ */
