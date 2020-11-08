#version 330

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

in vec3 vPos;

void main()
{
    gl_Position = uProjectionMatrix * ( uViewMatrix * vec4(vPos, 1.0) );
}
