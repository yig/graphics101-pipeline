#version 330

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
// uniform mat3 uNormalMatrix;
// uniform float uTime;

in vec3 vPos;

// out vec3 fNormal;

void main()
{
    gl_Position = uProjectionMatrix * ( uViewMatrix * vec4(vPos, 1.0) );
}
