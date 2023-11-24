#pragma once
#include "entity.h"
#include "renderer.h"
#include <zeta/physicshandler.h>
#define MAX_SCENES 10
namespace Scene{
    enum ScenePhase {
        SCENE_PHASE_SETUP,
        SCENE_PHASE_PAUSED,
        SCENE_PHASE_PLAYING
    };

    struct InitialEntityValues{
        void *entity_id;
        HMM_Vec3 position;
    };

    struct Scene{
        E_::Entity *entities[MAX_ENTITIES];
        InitialEntityValues iev[MAX_ENTITIES];
        int entity_count;
        ScenePhase phase;
        float time_paused;
        
        Zeta::Handler *physics_handler;
    };

    struct SceneManager{
        Scene scenes[MAX_SCENES];
        unsigned int index;
    };

    bool add_entity(Scene *scene, E_::Entity *entity);
    bool remove_entity(Scene *scene, E_::Entity *entity);

    Scene *new_scene(SceneManager *sm);

    void setup(Scene *scene);
    void play(Scene *scene, float &time_step);
    void pause(Scene *scene, float time_step);
    void reset(Scene *scene);
    void update(Scene *scene, float &time_step, RendererData *rd, Camera *camera, TexturesManager *tm, InputManager *im);
};
