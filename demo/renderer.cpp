#include "renderer.h"


void render(E_::Entity_ *entity, TexturesManager *textures_manager, Shader *shader){
    
    HMM_Mat4 transformation;
    if(entity->sb){
        transformation = HMM_Translate({entity->sb->pos.x, entity->sb->pos.y, entity->sb->pos.z});
    } else {
        transformation = HMM_Translate({entity->rb->pos.x, entity->rb->pos.y, entity->rb->pos.z});
    }
    
    transformation = HMM_Mul(transformation, HMM_Rotate_RH(HMM_ToRad(entity->rotation_x), HMM_Vec3{1.0f, 0.0f, 0.0f}));
    transformation = HMM_Mul(transformation, HMM_Rotate_RH(HMM_ToRad(entity->rotation_y), HMM_Vec3{0.0f, 1.0f, 0.0f}));
    transformation = HMM_Mul(transformation, HMM_Rotate_RH(HMM_ToRad(entity->rotation_z), HMM_Vec3{0.0f, 0.0f, 1.0f}));
    transformation = HMM_Mul(transformation, HMM_Scale(HMM_Vec3{entity->scale, entity->scale, entity->scale}));
    
    unsigned int u_transform_matrix = GetUniformLocation(shader, "transformation_matrix");
    SetUniformValue(u_transform_matrix, transformation);
    unsigned int u_entity_color = GetUniformLocation(shader, "u_color");
    SetUniformValue(u_entity_color, entity->color);

    if(entity->isTransparent){
        DisableCulling();
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

    }else{
        EnableCulling();
    }

    glBindVertexArray(entity->raw_model.vao_ID);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);    
    glEnableVertexAttribArray(2);    
    glEnableVertexAttribArray(3);    

    unsigned int u_texture_1 = GetUniformLocation(shader, "texture_1");
    SetUniformValue(u_texture_1, (int)0);
    unsigned int u_texture_2 = GetUniformLocation(shader, "texture_2");
    SetUniformValue(u_texture_2, (int)1);
    unsigned int u_texture_3 = GetUniformLocation(shader, "texture_3");
    SetUniformValue(u_texture_3, (int)2);
    unsigned int u_texture_4 = GetUniformLocation(shader, "texture_4");
    SetUniformValue(u_texture_4, (int)3);

    if(entity->textureIndex > 0){

        for(int i = 0; i < entity->textureIndex; i++){
            glActiveTexture(GL_TEXTURE0 + i + 1);
            glBindTexture(GL_TEXTURE_2D, textures_manager->GetTextureIdentifier(entity->textures[i]));
        } 

    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures_manager->GetTextureIdentifier(entity->def_texture));
    }


    glDrawElements(GL_TRIANGLES, entity->raw_model.vertex_count, GL_UNSIGNED_INT, 0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}
