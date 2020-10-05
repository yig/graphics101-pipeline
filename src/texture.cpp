#include "texture.h"

#include "glcompat.h"

#include <iostream>
using std::cerr;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace graphics101 {

void bind_textures( const TextureVec& textures ) {
    for( int index = 0; index < textures.size(); ++index ) {
        glActiveTexture( GL_TEXTURE0 + index );
        
        // We will have a null pointer if bindable_textures was called
        // with an unknown name.
        if( !textures[index] ) {
            cerr << "No texture\n";
            continue;
        }
        
        textures[index]->bind();
    }
}

TextureVec bindable_textures( TextureSet& textures, const std::vector< std::string >& names_in_bind_order )
{
    TextureVec result;
    result.reserve( names_in_bind_order.size() );
    
    for( const auto& name : names_in_bind_order ) {
        if( textures.find( name ) == textures.end() ) {
            cerr << "Unknown texture name: " << name << '\n';
            result.push_back( nullptr );
            continue;
        }
        
        result.push_back( textures[name] );
    }
    
    return result;
}

Texture::Texture()
{
    glGenTextures( 1, &m_textureName );
    // 0 is an invalid texture name. We should never see it.
    assert( 0 != m_textureName );
}
Texture::~Texture()
{
    glDeleteTextures( 1, &m_textureName );
    // Set it to zero for debugging purposes.
    m_textureName = 0;
}

}

namespace {
std::pair< int, int > upload_image( GLenum target, const std::string& path, bool flip = false ) {
    int width(-1), height(-1), num_channels(-1);
    stbi_set_flip_vertically_on_load( flip );
    unsigned char* data = stbi_load( path.c_str(), &width, &height, &num_channels, 4 );
    if( !data ) {
        cerr << "ERROR: Could not load texture: " << path << '\n';
        width = height = -1;
    } else {
        glTexImage2D( target, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
    }
    stbi_image_free( data );
    
    return std::make_pair( width, height );
}
}

namespace graphics101 {

Texture2D::Texture2D( const std::string& image_path )
    : m_image_path( image_path )
{
    reload();
}
void Texture2D::reload()
{
    glBindTexture( GL_TEXTURE_2D, m_textureName );
    
    // Set common parameters. We're not using mip-mapping.
    // From: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
    
    // Set common parameters.
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  0 );
    
    // Upload data.
    // TODO: If we know that the new and old data are the same size, we could use glTexSubImage2D();
    const auto width_height = upload_image( GL_TEXTURE_2D, m_image_path, true );
    width = width_height.first;
    height = width_height.second;
}
void Texture2D::bind()
{
    glBindTexture( GL_TEXTURE_2D, m_textureName );
}

TextureCube::TextureCube(
    const std::string& image_path_x_plus, const std::string& image_path_x_minus,
    const std::string& image_path_y_plus, const std::string& image_path_y_minus,
    const std::string& image_path_z_plus, const std::string& image_path_z_minus
    )
    : m_image_path_x_plus( image_path_x_plus ), m_image_path_x_minus( image_path_x_minus ),
    m_image_path_y_plus( image_path_y_plus ), m_image_path_y_minus( image_path_y_minus ),
    m_image_path_z_plus( image_path_z_plus ), m_image_path_z_minus( image_path_z_minus )
{
    reload();
}
void TextureCube::reload()
{
    // From: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_Cubemap_Texture
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_textureName );
    
    // Set common parameters.
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL,  0 );
    
    // Upload data for the six faces.
    // TODO: If we know that the new and old data are the same size, we could use glTexSubImage2D();
    upload_image( GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_image_path_x_plus );
    upload_image( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, m_image_path_x_minus );
    upload_image( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, m_image_path_y_plus );
    upload_image( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, m_image_path_y_minus );
    upload_image( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, m_image_path_z_plus );
    upload_image( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, m_image_path_z_minus );
}
void TextureCube::bind()
{
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_textureName );
}

Texture::TexturePtr Texture2D::makePtr( const std::string& image_path )
{
    return std::make_shared< Texture2D >( image_path );
}
Texture::TexturePtr TextureCube::makePtr(
        const std::string& image_path_x_plus, const std::string& image_path_x_minus,
        const std::string& image_path_y_plus, const std::string& image_path_y_minus,
        const std::string& image_path_z_plus, const std::string& image_path_z_minus
        )
{
    return std::make_shared< TextureCube >(
        image_path_x_plus, image_path_x_minus,
        image_path_y_plus, image_path_y_minus,
        image_path_z_plus, image_path_z_minus
        );
}

}
