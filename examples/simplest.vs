#version 330

in vec3 vPos;
out vec3 fColor;

void main()
{
    fColor = vPos*0.5 + 0.5;
    gl_Position = vec4(vPos, 1.0);
}
