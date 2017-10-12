#ifndef __glfwd_h__
#define __glfwd_h__

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace graphics101 {

// Forward declarations
class ShaderProgram;
struct UniformSet;
class VertexAndFaceArrays;
class Texture;
struct Drawable;

typedef std::shared_ptr< ShaderProgram > ShaderProgramPtr;
typedef std::shared_ptr< UniformSet > UniformSetPtr;
typedef std::shared_ptr< VertexAndFaceArrays > VertexAndFaceArraysPtr;
typedef std::shared_ptr< Texture > TexturePtr;
typedef std::unordered_map< std::string, TexturePtr > TextureSet;
typedef std::vector< TexturePtr > TextureVec;
typedef std::vector< std::string > StringVec;
typedef std::unique_ptr< Drawable > DrawablePtr;

}

#endif /* __glfwd_h__ */
