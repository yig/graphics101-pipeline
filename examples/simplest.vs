#version 330

in vec3 vPos;
out vec3 fColor;

void main()
{
    fColor = vPos;
    gl_Position = vec4(vPos, 1.0);
}
