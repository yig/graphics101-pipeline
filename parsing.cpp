#include "parsing.h"

#include "glcompat.h"
#include "shaderprogram.h"

#include <fstream>
#include <sstream>
#include <iostream>
using std::cerr;

namespace graphics101 {

// Adds the uniforms in `j` to the UniformSet `u`.
void parseUniforms( const json& j, UniformSet& u, StringVec& texture_names_in_bind_order ) {
    texture_names_in_bind_order.clear();
    
    for( json::const_iterator iter = j.begin(); iter != j.end(); ++iter ) {
        
        if( !iter.value().count("type") ) {
            cerr << "Uniform has no type: " << j << '\n';
            continue;
        }
        if( !iter.value().count("value") ) {
            cerr << "Uniform has no value: " << j << '\n';
            continue;
        }
        
        const std::string name = iter.key();
        const std::string type = iter.value()["type"];
        const json val = iter.value()["value"];
        
        if( type == "texture" ) {
            if( !val.is_string() ) {
                cerr << "Uniform value is not a string: " << iter.value() << '\n';
                continue;
            }
            
            u.storeUniformSampler( name );
            texture_names_in_bind_order.push_back( val.get<std::string>() );
        }
        else if( type == "1f" ) {
            if( !val.is_number() ) {
                cerr << "Uniform value is not a number: " << iter.value() << '\n';
                continue;
            }
            
            u.storeUniform( name, val.get<GLfloat>() );
        }
        else if( type == "1i" ) {
            if( !val.is_number() ) {
                cerr << "Uniform value is not a number: " << iter.value() << '\n';
                continue;
            }
            
            u.storeUniform( name, val.get<GLint>() );
        }
        else if( type == "2f" ) {
            if( !val.is_array() ) {
                cerr << "Uniform value is not an array: " << iter.value() << '\n';
                continue;
            }
            if( !( val.size() == 2 ) ) {
                cerr << "Uniform value array has length != 2: " << iter.value() << '\n';
                continue;
            }
            if( !( val[0].is_number() && val[1].is_number() ) ) {
                cerr << "Uniform value array does not contain numbers: " << iter.value() << '\n';
                continue;
            }
            
            u.storeUniform( name, vec2( val[0], val[1] ) );
        }
        else if( type == "2i" ) {
            if( !val.is_array() ) {
                cerr << "Uniform value is not an array: " << iter.value() << '\n';
                continue;
            }
            if( !( val.size() == 2 ) ) {
                cerr << "Uniform value array has length != 2: " << iter.value() << '\n';
                continue;
            }
            if( !( val[0].is_number() && val[1].is_number() ) ) {
                cerr << "Uniform value array does not contain numbers: " << iter.value() << '\n';
                continue;
            }
            
            u.storeUniform( name, ivec2( val[0], val[1] ) );
        }
        else if( type == "3f" ) {
            if( !val.is_array() ) {
                cerr << "Uniform value is not an array: " << iter.value() << '\n';
                continue;
            }
            if( !( val.size() == 3 ) ) {
                cerr << "Uniform value array has length != 3: " << iter.value() << '\n';
                continue;
            }
            if( !( val[0].is_number() && val[1].is_number() && val[2].is_number() ) ) {
                cerr << "Uniform value array does not contain numbers: " << iter.value() << '\n';
                continue;
            }
            
            u.storeUniform( name, vec3( val[0], val[1], val[2] ) );
        }
        else if( type == "3i" ) {
            if( !val.is_array() ) {
                cerr << "Uniform value is not an array: " << iter.value() << '\n';
                continue;
            }
            if( !( val.size() == 3 ) ) {
                cerr << "Uniform value array has length != 3: " << iter.value() << '\n';
                continue;
            }
            if( !( val[0].is_number() && val[1].is_number() && val[2].is_number() ) ) {
                cerr << "Uniform value array does not contain numbers: " << iter.value() << '\n';
                continue;
            }
            
            u.storeUniform( name, ivec3( val[0], val[1], val[2] ) );
        }
        else if( type == "4f" ) {
            if( !val.is_array() ) {
                cerr << "Uniform value is not an array: " << iter.value() << '\n';
                continue;
            }
            if( !( val.size() == 4 ) ) {
                cerr << "Uniform value array has length != 4: " << iter.value() << '\n';
                continue;
            }
            if( !( val[0].is_number() && val[1].is_number() && val[2].is_number() && val[3].is_number() ) ) {
                cerr << "Uniform value array does not contain numbers: " << iter.value() << '\n';
                continue;
            }
            
            u.storeUniform( name, vec4( val[0], val[1], val[2], val[3] ) );
        }
        else if( type == "4i" ) {
            if( !val.is_array() ) {
                cerr << "Uniform value is not an array: " << iter.value() << '\n';
                continue;
            }
            if( !( val.size() == 4 ) ) {
                cerr << "Uniform value array has length != 4: " << iter.value() << '\n';
                continue;
            }
            if( !( val[0].is_number() && val[1].is_number() && val[2].is_number() && val[3].is_number() ) ) {
                cerr << "Uniform value array does not contain numbers: " << iter.value() << '\n';
                continue;
            }
            
            u.storeUniform( name, vec4( val[0], val[1], val[2], val[3] ) );
        }
        else {
            cerr << "Uniform has unsupported type: " << j << '\n';
            continue;
        }
    }
}

// An anonymous namespace for helpers.
namespace {

std::string file_as_string( const std::string& path ) {
    // Open the file from the string path.
    std::ifstream infile( path );
    if( !infile ) {
        cerr << "ERROR: Unable to access path: " << path << '\n';
        return "";
    }
    
    // Short and efficient enough for shader-sized files.
    // From: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring/2602258#2602258
    std::stringstream buffer;
    buffer << infile.rdbuf();
    return buffer.str();
}
void plus_equals( StringSet& lhs, const StringSet& rhs ) {
    for( const auto& item : rhs ) { lhs.insert( item ); }
}
// Every path accessed is inserted into `paths_accessed_out`.
StringSet parseShaderType( const json& j, ShaderProgram& program, GLenum type, const StringTransformer& path_transformer ) {
    StringSet paths_accessed;
    
    // If it's a string, it's a shader.
    if( j.is_string() ) {
        program.addShader( type, j.get<std::string>() );
    }
    // If it's an array, each element is a path.
    // Load all the paths, each one is a piece of the shader.
    else if( j.is_array() ) {
        StringVec pieces;
        pieces.reserve( j.size() );
        // Load each path.
        for( const std::string& path : j ) {
            const auto fullpath = path_transformer( path );
            paths_accessed.insert( fullpath );
            pieces.push_back( file_as_string( fullpath ) );
        }
        program.addShader( type, pieces );
    } else {
        cerr << "ERROR: Unknown JSON data encountered when parsing shader type.\n";
    }
    
    return paths_accessed;
}
}

StringSet parseShader(
    const json& j,
    ShaderProgram& program,
    const StringTransformer& path_transformer
    ) {
    
    StringSet paths_accessed;
    
    // Add the various kinds of shaders:
    // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER
    if( j.count("vertex")          ) plus_equals( paths_accessed, parseShaderType( j["vertex"],          program, GL_VERTEX_SHADER,          path_transformer ) );
    if( j.count("fragment")        ) plus_equals( paths_accessed, parseShaderType( j["fragment"],        program, GL_FRAGMENT_SHADER,        path_transformer ) );
    if( j.count("geometry")        ) plus_equals( paths_accessed, parseShaderType( j["geometry"],        program, GL_GEOMETRY_SHADER,        path_transformer ) );
    if( j.count("tess_control")    ) plus_equals( paths_accessed, parseShaderType( j["tess_control"],    program, GL_TESS_CONTROL_SHADER,    path_transformer ) );
    if( j.count("tess_evaluation") ) plus_equals( paths_accessed, parseShaderType( j["tess_evaluation"], program, GL_TESS_EVALUATION_SHADER, path_transformer ) );
    // Compile and link.
    program.link();
    
    return paths_accessed;
}
StringSet parseShader( const json& j, ShaderProgram& program ) {
    return parseShader( j, program, []( const std::string& s ) { return s; } );
}

bool loadJSONFromPath( const std::string& path, json& json_out ) {
    // Open the file from the string path.
    std::ifstream infile( path );
    if( !infile ) {
        cerr << "ERROR: Unable to access path: " << path << '\n';
        return false;
    }
    
    // Parse the file contents into a json object.
    try {
        infile >> json_out;
    }
    catch( const std::exception& err ) {
        cerr << "ERROR: Invalid JSON: " << path << '\n';
        return false;
    }
    
    return true;
}

}
