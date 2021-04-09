#version 330

in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;
in vec2 vTexCoord;

out vec3 fNormal;
out vec3 fTangent;
out vec3 fBitangent;
out vec2 fTexCoord;

void main()
{
    fNormal = vNormal;
    fTangent = vTangent;
    fBitangent = vBitangent;
    fTexCoord = vTexCoord;
    
    // Position is the texture coordinate (stretched to fill the unit square [-1,1]).
    gl_Position = vec4( vTexCoord*2 - 1, 0, 1 );
}
