/*
*   This is a header-only graphics library written in C99, so although it does kinda have C++ support
*   all of the documentation is in C, so for now ill use the C-Style of writing the example
*   but I will refactor this in a more C++ way in the future. I miss classes :(
*/
#define GS_IMPL
#include "gunslinger/gs.h"
#define GS_IMMEDIATE_DRAW_IMPL
#include "gunslinger/util/gs_idraw.h"

#include "graphics_layer.h"
#include "graphics_layer.cpp"
#include "physicshandler.h"

typedef struct fps_camera_t {
    float pitch;
    float bob_time;
    gs_camera_t cam;
} fps_camera_t;

Primitives::Cube cube1(ZMath::Vec3D(-5, -5, -5), ZMath::Vec3D(5, 5, 5), 0, 0);
Primitives::Cube cube2(ZMath::Vec3D(-5, -5 + 20, -5), ZMath::Vec3D(5, 5 + 20, 5), 0, 0);
Primitives::Cube ground(ZMath::Vec3D(-40, -2, -40), ZMath::Vec3D(40, 2, 40), 0, 0);
PhysicsHandler::Object o1;
PhysicsHandler::Object o2;
PhysicsHandler::Handler handler(ZMath::Vec3D(0, -9.8f, 0));

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

    Primitives::Collider3D c1;
    c1.type = Primitives::CUBE_COLLIDER;
    c1.cube = cube1;

    Primitives::Collider3D c2;
    c2.type = Primitives::CUBE_COLLIDER;
    c2.cube = cube2;

    o1 = PhysicsHandler::Object(c1);
    o2 = PhysicsHandler::Object(c2);

    handler.addObject(o1);
    handler.addObject(o2);
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
        handler.update(dt);
        dt -= (float)(int)(dt/0.0167f) * 0.0167;
    }

    //std::cout << "It's time: " << o1.collider.type << " Second iteration: " << o2.collider.type << "\n";

    PhysicsHandler::Object* objs = handler.getObject();

    cube1 = objs[0].collider.cube;
    cube2 = objs[1].collider.cube ;
    DrawRectPrism(&appState, cube1.getVertices(), {cube1.rb.pos.x, cube1.rb.pos.x, cube1.rb.pos.z}, {225.0 / 255.0, 1.0 / 255.0, 1.0, 1.0});
    DrawRectPrism(&appState, cube2.getVertices(), {cube2.rb.pos.x, cube2.rb.pos.x, cube2.rb.pos.z}, {225.0 / 255.0, 1.0 / 255.0, 1.0, 1.0});

    delete[] objs;

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
