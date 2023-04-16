/*
*   This is a header-only graphics library written in C99, so although it does kinda have C++ support
*   all of the documentation is in C, so for now ill use the C-Style of writing the example
*   but I will refactor this in a more C++ way in the future. I miss classes :(
*/
#define GS_IMPL
#include "gunslinger/gs.h"
#define GS_IMMEDIATE_DRAW_IMPL
#include "gunslinger/util/gs_idraw.h"

#include "primitives.h"
#include "graphics_layer.h"
#include "graphics_layer.cpp"
#include "intersections.h"

typedef struct fps_camera_t {
    float pitch;
    float bob_time;
    gs_camera_t cam;
} fps_camera_t;

Primitives::Cube our_cube(ZMath::Vec3D(-5, -5, -5), ZMath::Vec3D(5, 5, 5), 0, 0);
Primitives::Cube cube_two(ZMath::Vec3D(-5, -5, -5), ZMath::Vec3D(5, 5, 5), 0, 0);
Primitives::Cube cube_three(ZMath::Vec3D(-5, -5, -5), ZMath::Vec3D(5, 5, 5), 0, 0);
Primitives::Cube ground(ZMath::Vec3D(-40, -2, -40), ZMath::Vec3D(40, 2, 40), 0, 0);

Primitives::Sphere our_sphere(0, ZMath::Vec3D(2, 2, 2));

gs_command_buffer_t cb = {0};
gs_immediate_draw_t gsi = {0};
fps_camera_t fps = {0};

float dt; // dt loop
AppState appState = {};

void init() {
    cb = gs_command_buffer_new();
    gsi = gs_immediate_draw_new();
    fps.cam = gs_camera_perspective();
    fps.cam.transform.position = gs_v3(4.f, 30.f, 20.f);

    gs_platform_lock_mouse(gs_platform_main_window(), true);    
    SetupScene(fps.cam);

    loadObjModel("sphere.obj");
}

void fps_camera_update(fps_camera_t* fps)
{
    gs_platform_t* platform = gs_subsystem(platform);

    gs_vec2 dp = gs_vec2_scale(gs_platform_mouse_deltav(), 0.01f);
    const float mod = gs_platform_key_down(GS_KEYCODE_LEFT_SHIFT) ? 2.f : 1.f; 
    dt = platform->time.delta;
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

    fps->cam.transform.position = gs_vec3_add(fps->cam.transform.position, gs_vec3_scale(gs_vec3_norm(vel), dt * 40 * mod));

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
        // our_cube.rb.theta += (3.0f * (int)(dt/0.0167f));
        // our_cube.rb.phi -= (1.5f * (int)(dt/0.0167f));
        dt -= (float)(int)(dt/0.0167f) * 0.0167;
    }
  

    // scene
    #if 0
    // testing VBO load by rendering 10,000 cubes
    float space = 15;
    int index = 0;
    gs_vec3 position = { space * 5 , 0, -5 * space};
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            position.x -= space;

            DrawRectPrism(&appState, our_cube.getVertices(), position, {rand_colors[index].x / 255.0f, rand_colors[index].y / 255.0f,rand_colors[index].z / 255.0f, 1.0f});
            index++;

            if(index >= 400){
                index = 0;
            }
        }
        position.z += space;
        position.x = space * 5;
    }
    #endif


    static ZMath::Vec3D testCubePosition = {15, 5, 0};
    float speed = 25;
    if (gs_platform_key_down(GS_KEYCODE_LEFT)){
        testCubePosition.x -= speed * dt; 
    }else if (gs_platform_key_down(GS_KEYCODE_RIGHT)){
        testCubePosition.x += speed * dt; 
    }

    if (gs_platform_key_down(GS_KEYCODE_UP)){
        testCubePosition.z -= speed * dt; 
    }else if (gs_platform_key_down(GS_KEYCODE_DOWN)){
        testCubePosition.z += speed * dt; 
    }
    
    // cube_two.rb.pos = {0, 5, 0};
    static ZMath::Vec3D testCubePositionTwo = {15, 6, 15};
    

    ZMath::Vec3D mirror_pos_one_min = testCubePosition - 5;
    ZMath::Vec3D mirror_pos_one_max = testCubePosition + 5;
    
    Primitives::Cube mirror_cube_1(mirror_pos_one_min, mirror_pos_one_max, our_cube.rb.theta, our_cube.rb.phi);

    ZMath::Vec3D mirror_pos_two_min = testCubePositionTwo - 5;
    ZMath::Vec3D mirror_pos_two_max = testCubePositionTwo + 5;
    
    Primitives::Cube mirror_cube_2(mirror_pos_two_min, mirror_pos_two_max, cube_two.rb.theta, cube_two.rb.phi);
    
    Primitives::AABB aabb_1(mirror_cube_1.getLocalMin(), mirror_cube_1.getLocalMax());
    Primitives::AABB aabb_2(mirror_cube_2.getLocalMin(), mirror_cube_2.getLocalMax());

    // drawing the cubes
    // if(Collisions::CubeAndCube(mirror_cube_1, mirror_cube_2)){
    if(Collisions::AABBAndAABB(aabb_1, aabb_2)){
        DrawRectPrism(&appState, mirror_cube_1.getVertices(), {mirror_cube_1.rb.pos.x, mirror_cube_1.rb.pos.y, mirror_cube_1.rb.pos.z}, {1.0f, 0.0f, 0.0f, 1.0f});
    }else{
        DrawRectPrism(&appState, mirror_cube_1.getVertices(), {mirror_cube_1.rb.pos.x, mirror_cube_1.rb.pos.y, mirror_cube_1.rb.pos.z}, {0.0f, 1.0f, 0.0f, 1.0f});
    }

    DrawRectPrism(&appState, mirror_cube_2.getVertices(), {mirror_cube_2.rb.pos.x, mirror_cube_2.rb.pos.y, mirror_cube_2.rb.pos.z}, {1.0f, 1.0f, 1.0f, 1.0f});

    // DrawRectPrism(&appState, cube_two.getVertices(), {cube_two.rb.pos.x, cube_two.rb.pos.y, cube_two.rb.pos.z}, {1.0f, 1.0f, 1.0f, 1.0f});
    // DrawRectPrism(&appState, cube_three.getVertices(), {-10, 0, 15}, {1.0f, 0.9f, 0.0f, 1.0f});
    DrawRectPrism(&appState, ground.getVertices(), {0, -10, 0}, {1.0f, 1.0f, 1.0f, 1.0f});
    UpdateScene(&appState, fps.cam);

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
