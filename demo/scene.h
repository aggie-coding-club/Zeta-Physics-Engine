#pragma once
#include "entity.h"
#include "renderer.h"
#include <zeta/physicshandler.h>

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
        int index;
        ScenePhase phase;
        
        Zeta::Handler *physics_handler;
    };

    bool add_entity(Scene *scene, E_::Entity *entity);
    bool remove_entity(Scene *scene, E_::Entity *entity);

    void setup(Scene *scene);
    void pause(Scene *scene);
    void reset(Scene *scene);
    void update(Scene *scene, float &time_step, RendererData *rd, Camera *camera, TexturesManager *tm, InputManager *im);
};
