#version 330

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

uniform mat4 uObjectToBone[30];
uniform mat4 uBoneToWorld[30];
uniform int uBoneIndex;

in vec3 vPos;

void main()
{
    gl_Position = uProjectionMatrix * ( uViewMatrix * ( uBoneToWorld[uBoneIndex] * ( uObjectToBone[uBoneIndex] * vec4(vPos, 1.0) )));
    // gl_Position = uProjectionMatrix * ( uViewMatrix * ( vec4(vPos, 1.0) ));
}
