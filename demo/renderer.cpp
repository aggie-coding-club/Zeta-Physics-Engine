#include "renderer.h"

HMM_Mat4 create_projection_matrix(RendererData *rd, int width, int height){
    HMM_Mat4 result = {};
    float near_plane = 0.1f;
    float far_plane = 1000.0f;

    result = HMM_Perspective_RH_ZO(HMM_DegToRad * rd->projection_fov, width/height, near_plane, far_plane);

    return result;
}

HMM_Mat4 create_view_matrix(HMM_Vec3 position, HMM_Vec3 front, HMM_Vec3 up){
    HMM_Mat4 result = {};

    result = HMM_LookAt_RH(position, position + front, up);

    return result;
}

void bind_fbo(FBO *fbo){
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->id);
    glViewport(0, 0, fbo->width, fbo->height);
}

void unbind_fbo(){
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

ShadowMapFBO create_shadow_map(unsigned int width, unsigned int height){
    ShadowMapFBO result = {};


    // creating depth buffer
    glGenTextures(1, &result.shadow_map);
    glBindTexture(GL_TEXTURE_2D, result.shadow_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // creating the framebuffer
    glGenFramebuffers(1, &result.fbo.id);
    glBindFramebuffer(GL_FRAMEBUFFER, result.fbo.id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, result.shadow_map, 0);

    // depth only fragment
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // error checking framebuffer
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);   
    if(status != GL_FRAMEBUFFER_COMPLETE){
        printf("FRAME BUFFER ERROR : 0x%x \n", status);
        result.fbo.id = -1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return result;
}

void lighting_pass_render(RendererData *rd, E_::Entity *entity, TexturesManager *textures_manager, Shader *shader){
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
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(shader->program);
    unsigned int u_transform_matrix = get_uniform_location(shader, (char *)"transformation_matrix");
    set_uniform_value(u_transform_matrix, transformation);
    unsigned int u_entity_color = get_uniform_location(shader, (char *)"u_color");
    set_uniform_value(u_entity_color, entity->color);

    if(entity->isTransparent){
        // disable_culling();
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

    }else{
        // enable_culling();
    }

    glBindVertexArray(entity->raw_model.vao_ID);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);    
    glEnableVertexAttribArray(2);    
    glEnableVertexAttribArray(3);    

    unsigned int u_texture_1 = get_uniform_location(shader, (char *)"texture_1");
    set_uniform_value(u_texture_1, (int)0);
    unsigned int u_texture_2 = get_uniform_location(shader, (char *)"texture_2");
    set_uniform_value(u_texture_2, (int)1);
    unsigned int u_texture_3 = get_uniform_location(shader, (char *)"texture_3");
    set_uniform_value(u_texture_3, (int)2);
    unsigned int u_texture_4 = get_uniform_location(shader, (char *)"texture_4");
    set_uniform_value(u_texture_4, (int)3);
    // unsigned int u_texture_shadow_map = get_uniform_location(shader, (char *)"texture_shadow_map");
    // set_uniform_value(u_texture_shadow_map, (int)5);

    if(entity->textureIndex > 0){

        for(int i = 0; i < entity->textureIndex; i++){
            glActiveTexture(GL_TEXTURE0 + i + 1);
            glBindTexture(GL_TEXTURE_2D, textures_manager->GetTextureIdentifier(entity->textures[i]));
        } 

    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures_manager->GetTextureIdentifier(entity->def_texture));
    }

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, rd->smf.shadow_map);

    glDrawElements(GL_TRIANGLES, entity->raw_model.vertex_count, GL_UNSIGNED_INT, 0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

void init_renderer(RendererData *rd){
    rd->smf = create_shadow_map(WINDOW_WIDTH, WINDOW_HEIGHT);
}


void prepare_renderer(RendererData *rd, Camera *camera){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(rd->main_shader.program);
    rd->view_matrix = create_view_matrix(camera->position, camera->front, camera->world_up); 
    rd->projection_matrix = create_projection_matrix(rd, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    unsigned int u_projection_matrix = get_uniform_location(&rd->main_shader, (char *)"projection_matrix");
    set_uniform_value(u_projection_matrix, rd->projection_matrix);
    
    unsigned int u_view_matrix = get_uniform_location(&rd->main_shader, (char *)"view_matrix");
    set_uniform_value(u_view_matrix, rd->view_matrix);
    
    unsigned int u_camera_position = get_uniform_location(&rd->main_shader, (char *)"camera_position");
    set_uniform_value(u_camera_position, camera->position);
    
    // unsigned int u_light_position = get_uniform_location(&rd->main_shader, (char *)"light_position");
    // set_uniform_value(u_light_position, HMM_Vec3{rd->main_light_pos.X, rd->main_light_pos.Y, rd->main_light_pos.Z});
    
    unsigned int u_light_color = get_uniform_location(&rd->main_shader, (char *)"light_color");
    set_uniform_value(u_light_color, HMM_Vec3{1.0f, 1.0f, 1.0f});

    unsigned int u_light_direction = get_uniform_location(&rd->main_shader, (char *)"light_direction");
    set_uniform_value(u_light_direction, HMM_Vec3{30.0f, 15.0f, 15.0f});
    glUseProgram(0);
}

void prepare_shadow_renderer(RendererData *rd){
    glViewport(0,0, rd->smf.fbo.width, rd->smf.fbo.height);
    glUseProgram(rd->shadow_map_shader.program);
    bind_fbo(&rd->smf.fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    rd->shadow_view_matrix = create_view_matrix(rd->main_light_pos, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    rd->shadow_projection_matrix = create_projection_matrix(rd, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    unsigned int u_projection_matrix = get_uniform_location(&rd->main_shader, (char *)"projection_matrix");
    set_uniform_value(u_projection_matrix, rd->shadow_projection_matrix);
    
    unsigned int u_view_matrix = get_uniform_location(&rd->main_shader, (char *)"view_matrix");
    set_uniform_value(u_view_matrix, rd->shadow_view_matrix);

    unbind_fbo();
    glUseProgram(0);
}

void shadow_pass_render(RendererData *rd, E_::Entity *entity, TexturesManager *textures_manager, Shader *shader){
    bind_fbo(&rd->smf.fbo);
    glUseProgram(rd->shadow_map_shader.program);
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
    
    unsigned int u_transform_matrix = get_uniform_location(shader, (char *)"transformation_matrix");
    set_uniform_value(u_transform_matrix, transformation);

#if 0
    if(entity->isTransparent){
        disable_culling();
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

    }else{
        enable_culling();
    }
#endif

    glBindVertexArray(entity->raw_model.vao_ID);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, entity->raw_model.vertex_count, GL_UNSIGNED_INT, 0);
    
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);

    glUseProgram(0);
    unbind_fbo();
}


void render(RendererData *rd, Camera *camera, E_::Entity *entity, TexturesManager *textures_manager, Shader *shader){
    lighting_pass_render(rd, entity, textures_manager, shader);
    shadow_pass_render(rd, entity, textures_manager, shader);
}

void render_entities(RendererData *rd, Camera *camera, E_::Entity *entities, TexturesManager *tm){
    
    #if 0
    prepare_shadow_renderer(rd);
    // // shadow pass
    for(int i = 0; i < MAX_ENTITIES; i++){
        E_::Entity *entity = &entities[i];
        if(entity->initialized == true){
            shadow_pass_render(rd, entity, tm, &rd->main_shader);
        }
    }

    #endif
     
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, rd->db_tex);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glReadBuffer(GL_BACK); // reading from backbuffer
    // glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    // glBindTexture(GL_TEXTURE_2D, 0);


    // lighting pass
    prepare_renderer(rd, camera);
    for(int i = 0; i < MAX_ENTITIES; i++){
        E_::Entity *entity = &entities[i];
        if(entity->initialized == true){
            lighting_pass_render(rd, entity, tm, &rd->main_shader); 
        }
    }
    
}