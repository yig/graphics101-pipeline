#version 330

// Eye-space.
in vec3 fPos;
in vec3 fNormal;
in vec2 fTexCoord;

uniform mat3 uNormalMatrix;

uniform struct Material {
    vec3 color;
    float shininess;
    int bands;
    
    bool use_diffuse_texture;
    sampler2D diffuse_texture;
} material;

struct Light {
    // Eye-space.
    vec3 position;
    float color;
    float color_ambient;
};
const int MAX_LIGHTS = 5;
uniform Light lights[ MAX_LIGHTS ];

uniform int num_lights;

// gl_FragColor is old-fashioned, but it's what WebGL 1 uses.
// From: https://stackoverflow.com/questions/9222217/how-does-the-fragment-shader-know-what-variable-to-use-for-the-color-of-a-pixel
layout(location = 0) out vec4 FragColor;

void main()
{
    // Your code goes here.
    
    FragColor = vec4( 1.0, 0.0, 1,0, 1.0 );
}
