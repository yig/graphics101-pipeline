#version 330

// Eye-space.
in vec3 fNormal;

in vec2 fTexCoord;

uniform struct Material {
    sampler2D matcap_texture;
    
    bool use_diffuse_texture;
    sampler2D diffuse_texture;
    
    bool use_normal_map;
    sampler2D normal_map;
} material;

uniform mat3 uNormalMatrix;

// gl_FragColor is old-fashioned, but it's what WebGL 1 uses.
// From: https://stackoverflow.com/questions/9222217/how-does-the-fragment-shader-know-what-variable-to-use-for-the-color-of-a-pixel
layout(location = 0) out vec4 FragColor;

void main()
{
    // Your code goes here.
    vec4 color = vec4( 1.0, 0.0, 1.0, 1.0 );
    
    FragColor = clamp( color, 0.0, 1.0 );
    // FragColor = vec4( normalize(fNormal)*0.5 + 0.5, 1.0 );
}
