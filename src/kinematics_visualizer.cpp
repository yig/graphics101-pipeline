#include "kinematics_visualizer.h"
#include "drawable.h"
#include "parsing.h"
#include "vao.h"
#include <cmath> // acos
#include "glcompat.h"

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
    m_drawable->program->addShader( GL_VERTEX_SHADER, fileAsString( pathRelativeToFile( scene_path, "bone.vs" ) ) );
    m_drawable->program->addShader( GL_FRAGMENT_SHADER, fileAsString( pathRelativeToFile( scene_path, "bone.fs" ) ) );
    m_drawable->program->link();
    
    // Upload the `bone.obj` Mesh data.
    m_drawable->vao = vao::makeFromOBJPath(
        pathRelativeToFile( scene_path, "bone.obj" ),
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
        // Skip bones with no parents (roots). There is nothing to draw.
        if( skeleton.at(i).parent_index == -1 ) continue;
        
        const vec3 bone_end = skeleton.at(i).end;
        const vec3 bone_start = skeleton.at( skeleton.at(i).parent_index ).end;
        vec3 bone_vec = bone_end - bone_start;
        const real bone_length = length(bone_vec);
        bone_vec /= bone_length;
        
        object2bone.push_back(
            // Scale to the length from the bone position to its parent's position.
            // A glm transform function takes a matrix paramater P and returns PQ, where Q is the
            // requested transform.
            glm::scale(
                glm::rotate(
                    glm::translate( mat4(1),
                        bone_start
                        ),
                    acos( glm::clamp( glm::dot( vec3(0,0,1), bone_vec ), -1.f, 1.f ) ),
                    glm::cross( vec3(0,0,1), bone_vec )
                    ),
                vec3(bone_length)
                )
            );
    }
    
    // Upload the matrices to the GPU.
    m_drawable->program->setUniform( "uObjectToBone", object2bone );
}

void KinematicsVisualizer::setProjectionMatrix( const mat4& projection ) {
    assert( m_drawable );
    
    m_drawable->uniforms.storeUniform( "uProjectionMatrix", projection );
}
void KinematicsVisualizer::setViewMatrix( const mat4& view ) {
    m_drawable->uniforms.storeUniform( "uViewMatrix", view );
    m_drawable->uniforms.storeUniform( "uNormal", glm::inverse( glm::transpose( mat3(view) ) ) );
}
void KinematicsVisualizer::setPose( const MatrixPose& bone2world ) {
    // Set uniforms, a matrix for each bone that transforms from bone2world.
    m_drawable->program->setUniform( "uBoneToWorld", bone2world );
}

void KinematicsVisualizer::draw() {
    m_drawable->bind();
    
    // Turn on wireframe drawing mode.
    // TODO: Alternatively, do it in the fragment shader.
    // See: https://stackoverflow.com/questions/137629/how-do-you-render-primitives-as-wireframes-in-opengl/33004265#33004265
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    for( const auto index : m_draw_bone_indices ) {
        m_drawable->uniforms.storeUniform( "uBoneIndex", index );
        m_drawable->draw();
    }
    
    // Turn off wireframe drawing mode.
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

}
