#version 330

in vec3 vNormal;
in vec2 vTexCoord;

out vec3 fNormal;

void main()
{
    fNormal = vNormal;
    
    // Position is the texture coordinate (stretched to fill the unit square [-1,1]).
    gl_Position = vec4( vTexCoord*2 - 1, 0, 1 );
}
