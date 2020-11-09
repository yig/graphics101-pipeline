#include "kinematics_visualizer.h"
#include "drawable.h"
#include "parsing.h"
#include "vao.h"
#include <cmath> // acos
#include "glcompat.h"

#include "debugging.h"
#include <iostream>

#include <glm/ext/matrix_transform.hpp> // translate, rotate, scale

namespace graphics101 {

KinematicsVisualizer::KinematicsVisualizer() {}
KinematicsVisualizer::KinematicsVisualizer( const std::string& asset_dir, const Skeleton& skeleton ) {
    reset( asset_dir, skeleton );
}

void KinematicsVisualizer::reset() {
    m_draw_bone_indices.clear();
    m_drawable.reset();
}
void KinematicsVisualizer::reset( const std::string& scene_path, const Skeleton& skeleton ) {
    // Create a Drawable.
    m_drawable = Drawable::makePtr();
    
    // Create a shader program.
    // Everyone else expects it to have been created so we can get binding locations.
    m_drawable->program = ShaderProgram::makePtr();
    m_drawable->program->addShader( GL_VERTEX_SHADER, fileAsString( pathRelativeToFile( "bone.vs", scene_path ) ) );
    m_drawable->program->addShader( GL_FRAGMENT_SHADER, fileAsString( pathRelativeToFile( "bone.fs", scene_path ) ) );
    m_drawable->program->link();
    
    // Upload the `bone.obj` Mesh data.
    m_drawable->vao = vao::makeFromOBJPath(
        pathRelativeToFile( "bone.obj", scene_path ),
        // Don't create normals if not present.
        false,
        // Don't normalize.
        false,
        m_drawable->program->getAttribLocation( "vPos" ),
        m_drawable->program->getAttribLocation( "vNormal" )
        );
    
    // Set uniforms, a matrix for each bone that transforms from object to bone.
    // Skip the root.
    std::vector< mat4 > object2bone;
    object2bone.reserve( skeleton.size() );
    m_draw_bone_indices.clear();
    m_draw_bone_indices.reserve( skeleton.size() );
    for( int i = 0; i < skeleton.size(); ++i ) {
        // Only draw bones with parents. Roots have nothing to draw.
        if( skeleton.at(i).parent_index == -1 ) {
            // We still need a matrix for each bone, though, for compatible indexing
            // with bone2world.
            object2bone.push_back( mat4(1) );
            continue;
        }
        
        // Draw this bone.
        m_draw_bone_indices.push_back(i);
        
        const vec3 bone_end = skeleton.at(i).end;
        const vec3 bone_start = skeleton.at( skeleton.at(i).parent_index ).end;
        vec3 bone_vec = bone_end - bone_start;
        const real bone_length = length(bone_vec);
        bone_vec /= bone_length;
        const real cos_theta = glm::dot( vec3(0,0,1), bone_vec );
        // If the dot product is 1 or -1, they are already parallel. The cross product
        // will be zero, which will result in nan when normalizing it.
        // Use any axis in that case.
        const vec3 axis = fabs(fabs(cos_theta) - 1) > 1e-5
            ? glm::cross( vec3(0,0,1), bone_vec )
            : vec3(0,0,1)
            ;
        
        object2bone.push_back(
            // Scale to the length from the bone position to its parent's position.
            // A glm transform function takes a matrix paramater P and returns PQ, where Q is the
            // requested transform.
            glm::scale(
                glm::rotate(
                    glm::translate( mat4(1),
                        bone_start
                        ),
                    acos( glm::clamp( cos_theta, -1.f, 1.f ) ),
                    axis
                    ),
                vec3(bone_length)
                )
            );
    }
    
    // Upload the matrices to the GPU.
    m_drawable->program->setUniform( "uObjectToBone", object2bone );
    
    // For debugging:
    std::cout << "Object-to-bone matrices (column-at-a-time):\n";
    for( const mat4& m : object2bone ) {
        std::cout << m << '\n';
    }
}

void KinematicsVisualizer::setProjectionMatrix( const mat4& projection ) {
    if( !m_drawable ) return;
    
    m_drawable->uniforms.storeUniform( "uProjectionMatrix", projection );
}
void KinematicsVisualizer::setViewMatrix( const mat4& view ) {
    if( !m_drawable ) return;
    
    m_drawable->uniforms.storeUniform( "uViewMatrix", view );
    m_drawable->uniforms.storeUniform( "uNormalMatrix", glm::inverse( glm::transpose( mat3(view) ) ) );
}
void KinematicsVisualizer::setPose( const MatrixPose& bone2world ) {
    // This shouldn't be called if a skeleton was never given.
    assert( m_drawable );
    
    // Set uniforms, a matrix for each bone that transforms from bone2world.
    m_drawable->uniforms.storeUniform( "uBoneToWorld", bone2world );
    
    std::cout << "Bone-to-world matrices (column-at-a-time):\n";
    for( const mat4& m : bone2world ) {
        std::cout << m << '\n';
    }
}

void KinematicsVisualizer::draw() {
    if( !m_drawable ) return;
    
    m_drawable->bind();
    
    // Turn on wireframe drawing mode.
    // TODO: Alternatively, do it in the fragment shader.
    // See: https://stackoverflow.com/questions/137629/how-do-you-render-primitives-as-wireframes-in-opengl/33004265#33004265
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    for( const int index : m_draw_bone_indices ) {
        m_drawable->uniforms.storeUniform( "uBoneIndex", index );
        m_drawable->draw();
    }
    
    // Turn off wireframe drawing mode.
    // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

}
