#version 330

in vec3 fNormal;

// gl_FragColor is old-fashioned, but it's what WebGL 1 uses.
// From: https://stackoverflow.com/questions/9222217/how-does-the-fragment-shader-know-what-variable-to-use-for-the-color-of-a-pixel
layout(location = 0) out vec4 FragColor;

void main()
{
    // Color is the normal (packed into the range [0,1]).
    FragColor = vec4( normalize(fNormal)*0.5 + 0.5, 1.0 );
}
