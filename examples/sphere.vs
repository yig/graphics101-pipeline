#version 330

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat3 uNormalMatrix;
uniform float uTime;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 fNormal;
out vec2 fTexCoord;

void main()
{
    // Pass the normal in eye-space:
    // fNormal = uNormalMatrix * vNormal;
    // Pass the normal in world-space:
    fNormal = vNormal;
    
    fTexCoord = vTexCoord;
    
    // Pulse the position and then convert it to eye-space.
    float pulse = 1 + .1*sin(uTime);
    gl_Position = uProjectionMatrix * ( uViewMatrix * vec4(pulse*vPos, 1.0) );
}
