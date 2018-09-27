#version 330

in vec3 fNormal;
// in vec2 fTexCoord;

uniform vec3 uColor;
// uniform sampler2D uTex;
// uniform samplerCube uTexCube;

// gl_FragColor is old-fashioned, but it's what WebGL 1 uses.
// From: https://stackoverflow.com/questions/9222217/how-does-the-fragment-shader-know-what-variable-to-use-for-the-color-of-a-pixel
layout(location = 0) out vec4 FragColor;

void main()
{
    // Color is the 2D texture.
    // FragColor = texture( uTex, fTexCoord.xy ).rgba;
    
    // Color is the cube map.
    // FragColor = texture( uTexCube, normalize(fNormal) ).rgba;
    
    // Color is the normal.
    // FragColor = vec4( normalize(fNormal)*0.5 + 0.5, 1.0 );
    // FragColor = vec4( normalize(fNormal), 1.0 );
    
    // Color is the normal dotted with the y direction
    // float diffuse = dot( normalize(fNormal), vec3( 0,1,0 ) );
    // FragColor = vec4( diffuse*uColor, 1.0 );
    // FragColor = vec4( diffuse, diffuse, diffuse, 1.0 );
    
    // Color is purple.
    FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
}
