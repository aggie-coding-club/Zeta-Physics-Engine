/*
*   This is a header-only graphics library written in C99, so although it does kinda have C++ support
*   all of the documentation is in C, so for now ill use the C-Style of writing the example
*   but I will refactor this in a more C++ way in the future. I miss classes :(
*/
#define GS_IMPL
#include "gs/gs.h"
#define GS_IMMEDIATE_DRAW_IMPL
#include "gs/util/gs_idraw.h"

#include "primitives.h"
#include "zmath.h"

typedef struct fps_camera_t {
    float pitch;
    float bob_time;
    gs_camera_t cam;
} fps_camera_t;

Primitives::Cube our_cube;
Primitives::Sphere our_sphere;

gs_command_buffer_t cb = {0};
gs_immediate_draw_t gsi = {0};
fps_camera_t fps = {0};

float dt; // dt loop

void init() {
    cb = gs_command_buffer_new();
    gsi = gs_immediate_draw_new();
    fps.cam = gs_camera_perspective();
    fps.cam.transform.position = gs_v3(4.f, 2.f, 4.f);

    our_cube = Primitives::Cube(ZMath::Vec3D(-2, -2, -2), ZMath::Vec3D(2, 2, 2), 45, 45);
    our_sphere = Primitives::Sphere();

    gs_platform_lock_mouse(gs_platform_main_window(), true);

    // AABB:GetVerticies() debugging

    Primitives::AABB testAABB = Primitives::AABB({100, 100, 0}, {200, 200, 0});

    for(int i = 0; i < 4; i++){
        ZMath::Vec3D vertice = testAABB.getVertices()[i];

        printf("x : %f, y : %f \n", vertice.x, vertice.y);
    }
    
}

void fps_camera_update(fps_camera_t* fps)
{
    gs_platform_t* platform = gs_subsystem(platform);

    gs_vec2 dp = gs_vec2_scale(gs_platform_mouse_deltav(), 0.01f);
    const float mod = gs_platform_key_down(GS_KEYCODE_LEFT_SHIFT) ? 2.f : 1.f; 
    float dt = platform->time.delta;
    float old_pitch = fps->pitch;

    // Keep track of previous amount to clamp the camera's orientation
    fps->pitch = gs_clamp(old_pitch + dp.y, -90.f, 90.f);

    // Rotate camera
    gs_camera_offset_orientation(&fps->cam, -dp.x, old_pitch - fps->pitch);

    gs_vec3 vel = {0};
    if (gs_platform_key_down(GS_KEYCODE_W)) vel = gs_vec3_add(vel, gs_camera_forward(&fps->cam));
    if (gs_platform_key_down(GS_KEYCODE_S)) vel = gs_vec3_add(vel, gs_camera_backward(&fps->cam));
    if (gs_platform_key_down(GS_KEYCODE_A)) vel = gs_vec3_add(vel, gs_camera_left(&fps->cam));
    if (gs_platform_key_down(GS_KEYCODE_D)) vel = gs_vec3_add(vel, gs_camera_right(&fps->cam));

    // For a non-flying first person camera, need to lock the y movement velocity
    vel.y = 0.f;

    fps->cam.transform.position = gs_vec3_add(fps->cam.transform.position, gs_vec3_scale(gs_vec3_norm(vel), dt * 5 * mod));

    // // If moved, then we'll "bob" the camera some
    // if (gs_vec3_len(vel) != 0.f) {
    //     fps->bob_time += dt * 8.f;
    //     float sb = sin(fps->bob_time);
    //     float bob_amt = (sb * 0.5f + 0.5f) * 0.1f * mod;
    //     float rot_amt = sb * 0.0004f * mod;
    //     fps->cam.transform.position.y = 2.f + bob_amt;        
    //     fps->cam.transform.rotation = gs_quat_mul(fps->cam.transform.rotation, gs_quat_angle_axis(rot_amt, GS_ZAXIS));
    // }
}

void update() {
    const gs_vec2 fbs = gs_platform_framebuffer_sizev(gs_platform_main_window());
    if (gs_platform_key_pressed(GS_KEYCODE_ESC)) gs_quit();

    // If click, then lock again (in case lost)
    if (gs_platform_mouse_pressed(GS_MOUSE_LBUTTON) && !gs_platform_mouse_locked()) {
        fps.cam.transform.rotation = gs_quat_default();
        fps.pitch = 0.f;
        gs_platform_lock_mouse(gs_platform_main_window(), true);
    }
    // Update camera
    if (gs_platform_mouse_locked()) {
        fps_camera_update(&fps);
    }

    // Update the cube
    if (dt >= 0.0167f) {
        our_cube.setTheta(our_cube.getTheta() + (3.0f * (int)(dt/0.0167f)));
        our_cube.setPhi(our_cube.getPhi() - (1.5f * (int)(dt/0.0167f)));
        dt = 0.0f;
    }

    gsi_depth_enabled(&gsi, true);
    gsi_face_cull_enabled(&gsi, true);
    gsi_camera(&gsi, &fps.cam, (uint32_t)fbs.x, (uint32_t)fbs.y);
    // gsi_push_matrix(&gsi, GSI_MATRIX_MODELVIEW);

    {
        ZMath::Vec3D* v = our_cube.getVertices();

        gsi_translatef(&gsi, 0.f, 0.f, -10.f);
        
        gsi_line3D(&gsi, v[0].x, v[0].y, v[0].z, v[1].x, v[1].y, v[1].z, 225, 1, 1, 255);
        gsi_line3D(&gsi, v[1].x, v[1].y, v[1].z, v[2].x, v[2].y, v[2].z, 225, 1, 1, 255);
        gsi_line3D(&gsi, v[2].x, v[2].y, v[2].z, v[3].x, v[3].y, v[3].z, 225, 1, 1, 255);
        gsi_line3D(&gsi, v[3].x, v[3].y, v[3].z, v[0].x, v[0].y, v[0].z, 225, 1, 1, 255);

        gsi_line3D(&gsi, v[1].x, v[1].y, v[1].z, v[4].x, v[4].y, v[4].z, 225, 1, 1, 255);
        gsi_line3D(&gsi, v[0].x, v[0].y, v[0].z, v[5].x, v[5].y, v[5].z, 225, 1, 1, 255);
        gsi_line3D(&gsi, v[3].x, v[3].y, v[3].z, v[6].x, v[6].y, v[6].z, 225, 1, 1, 255);
        gsi_line3D(&gsi, v[2].x, v[2].y, v[2].z, v[7].x, v[7].y, v[7].z, 225, 1, 1, 255);

        gsi_line3D(&gsi, v[4].x, v[4].y, v[4].z, v[7].x, v[7].y, v[7].z, 225, 1, 1, 255);
        gsi_line3D(&gsi, v[7].x, v[7].y, v[7].z, v[6].x, v[6].y, v[6].z, 225, 1, 1, 255);
        gsi_line3D(&gsi, v[6].x, v[6].y, v[6].z, v[5].x, v[5].y, v[5].z, 225, 1, 1, 255);
        gsi_line3D(&gsi, v[5].x, v[5].y, v[5].z, v[4].x, v[4].y, v[4].z, 225, 1, 1, 255);
        
        delete[] v;
    }

    {
        ZMath::Vec3D pos = our_sphere.getCenter();
        float r = our_sphere.getRadius();

        gsi_sphere(&gsi, pos.x, pos.y, pos.z, r, 255, 200, 100, 255, GS_GRAPHICS_PRIMITIVE_LINES);
    }

    // Draw text
    gsi_defaults(&gsi);
    gsi_camera2D(&gsi, fbs.x, fbs.y);
    gsi_rectvd(&gsi, gs_v2(10.f, 10.f), gs_v2(220.f, 70.f), gs_v2(0.f, 0.f), gs_v2(1.f, 1.f), gs_color(10, 50, 150, 128), GS_GRAPHICS_PRIMITIVE_TRIANGLES);
    gsi_rectvd(&gsi, gs_v2(10.f, 10.f), gs_v2(220.f, 70.f), gs_v2(0.f, 0.f), gs_v2(1.f, 1.f), gs_color(10, 50, 220, 255), GS_GRAPHICS_PRIMITIVE_LINES);
    gsi_text(&gsi, 20.f, 25.f, "FPS Camera Controls:", NULL, false, 0, 0, 0, 255);
    gsi_text(&gsi, 40.f, 40.f, "- Move: W, A, S, D", NULL, false, 20, 20, 20, 255);
    gsi_text(&gsi, 40.f, 55.f, "- Mouse to look", NULL, false, 20, 20, 20, 255);
    gsi_text(&gsi, 40.f, 70.f, "- Shift to run", NULL, false, 20, 20, 20, 255);

    // gsi_pop_matrix(&gsi);
    gsi_renderpass_submit(&gsi, &cb, gs_v4(0.f, 0.f, fbs.x, fbs.y), gs_color(10, 10, 10, 255));
    gs_graphics_command_buffer_submit(&cb);

    dt += gs_platform_delta_time();
}

gs_app_desc_t gs_main(int32_t argc, char **argv) {
    gs_app_desc_t desc = {};
    desc.init = init;
    desc.update = update;
    desc.window.title = "Zeta Physics Engine Example";
    desc.window.frame_rate = 60.0;

    return desc;
}
