#version 330

in vec3 fColor;

// gl_FragColor is old-fashioned, but it's what WebGL 1 uses.
// From: https://stackoverflow.com/questions/9222217/how-does-the-fragment-shader-know-what-variable-to-use-for-the-color-of-a-pixel
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4( fColor, 1.0 );
    // FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
}
