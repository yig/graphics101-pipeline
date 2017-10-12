#ifndef __texture_h__
#define __texture_h__

#include "types.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <memory> // shared_ptr

namespace graphics101 {

class Texture {
public:
    typedef std::shared_ptr< Texture > TexturePtr;
    
    Texture();
    virtual ~Texture();

    // Bind the texture to the active texture unit.
    // Call glActiveTexture() to set the desired texture unit before calling this.
    virtual void bind() = 0;
    
    // Reload image data from paths.
    virtual void reload() = 0;
    
    // This class cannot be copied. Use a TexturePtr.
    Texture( const Texture& ) = delete;
    void operator=( const Texture& ) = delete;
    
protected:
    GLuint m_textureName;
};

// Load a `TextureSet`.
// For each drawable, extract a `TextureVec` using `bindable_textures()`.
// Call `bind_textures` to before drawing to use them.
typedef std::unordered_map< std::string, Texture::TexturePtr > TextureSet;
typedef std::vector< Texture::TexturePtr > TextureVec;
TextureVec bindable_textures( const TextureSet& textures, const std::vector< std::string >& names_in_bind_order );
void bind_textures( const TextureVec& textures );

class Texture2D : public Texture {
public:
    Texture2D( const std::string& image_path );
    static TexturePtr makePtr( const std::string& image_path );

    void reload() override;

    void bind() override;
    
    // Access the dimensions of the image.
    int width = -1;
    int height = -1;

private:
    std::string m_image_path;
};

class TextureCube : public Texture {
public:
    TextureCube(
        const std::string& image_path_x_plus, const std::string& image_path_x_minus,
        const std::string& image_path_y_plus, const std::string& image_path_y_minus,
        const std::string& image_path_z_plus, const std::string& image_path_z_minus
        );
    static TexturePtr makePtr(
        const std::string& image_path_x_plus, const std::string& image_path_x_minus,
        const std::string& image_path_y_plus, const std::string& image_path_y_minus,
        const std::string& image_path_z_plus, const std::string& image_path_z_minus
        );
    
    void bind() override;
    
    void reload() override;
    
private:
    std::string m_image_path_x_plus;
    std::string m_image_path_x_minus;
    std::string m_image_path_y_plus;
    std::string m_image_path_y_minus;
    std::string m_image_path_z_plus;
    std::string m_image_path_z_minus;
};

}

#endif /* __texture_h__ */
