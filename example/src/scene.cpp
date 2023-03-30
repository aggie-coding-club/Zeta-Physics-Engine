#include "scene.h"


void SetupScene(){

}

void UpdateScene(){
    
    // gs_graphics_renderpass_begin(&cb, GS_GRAPHICS_RENDER_PASS_DEFAULT); 
    
    // {
        // gs_graphics_pipeline_bind(&cb, pip);
        // gs_graphics_set_viewport(&cb, 0, 0, (uint32_t)fs.x, (uint32_t)fs.y);
        // gs_graphics_clear(&cb, &clear);
        // gs_graphics_apply_bindings(&cb, &binds);

        // gs_vec3 test_cube_pos = {0.0f, 0.0f, 0.0f};
        // gs_mat4 model = gs_mat4_translate(test_cube_pos.x, test_cube_pos.y, test_cube_pos.z);
        
        // model = gs_mat4_mul(model, gs_mat4_scale(4.0f, 1.0f, 4.0f));

        // gs_graphics_bind_desc_t model_binds = {
        //     .uniforms = &(gs_graphics_bind_uniform_desc_t){.uniform = u_model, .data = &model}
        // };
        
        // gs_graphics_apply_bindings(&cb, &model_binds);
        // gs_graphics_draw(&cb, &(gs_graphics_draw_desc_t){.start = 0, .count = 36});

        // model = gs_mat4_translate(test_cube_pos.x, test_cube_pos.y, test_cube_pos.z);
        // model = gs_mat4_mul(model, gs_mat4_scale(1.0f, 4.0f, 1.0f));
        
        // gs_graphics_bind_desc_t model_binds_ = {
        //     .uniforms = &(gs_graphics_bind_uniform_desc_t){.uniform = u_model, .data = &model}
        // };

        // gs_graphics_apply_bindings(&cb, &model_binds_);
        // gs_graphics_draw(&cb, &(gs_graphics_draw_desc_t){.start = 0, .count = 36});
    // }

    // gs_graphics_renderpass_end(&cb);

    // Submit command buffer (syncs to GPU, MUST be done on main thread where you have your GPU context created)
    // gs_graphics_command_buffer_submit(&cb);
}