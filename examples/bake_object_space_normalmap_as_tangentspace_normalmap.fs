#version 330

in vec3 fNormal;
in vec3 fTangent;
in vec3 fBitangent;
in vec2 fTexCoord;

uniform sampler2D object_space_normal_map;

// gl_FragColor is old-fashioned, but it's what WebGL 1 uses.
// From: https://stackoverflow.com/questions/9222217/how-does-the-fragment-shader-know-what-variable-to-use-for-the-color-of-a-pixel
layout(location = 0) out vec4 FragColor;

void main()
{
    mat3 tangent_space_to_object_space = mat3(
        normalize( fTangent ),
        normalize( fBitangent ),
        normalize( fNormal )
        );
    
    // We can't use fNormal, because it always points exactly in the tangent-space (0,0,1) direction.
    // vec3 object_space_normal = fNormal;
    vec3 object_space_normal = 2*texture( object_space_normal_map, fTexCoord ).rgb - 1;
    vec3 tangent_space_normal = normalize( inverse( tangent_space_to_object_space ) * object_space_normal );
    
    // Color is the normal (packed into the range [0,1]).
    FragColor = vec4( tangent_space_normal*0.5 + 0.5, 1.0 );
}
