#pragma once
#include "entity.h"
#include "renderer.h"
#include <zeta/physicshandler.h>

#define SCENE_PHASE_SETUP 0x1
#define SCENE_PHASE_PAUSED 0x2
#define SCENE_PHASE_PLAYING 0x2

namespace Scene{

    struct InitialEntityValues{
        void *entity_id;
        HMM_Vec3 position;
    };

    struct Scene{
        E_::Entity *entities[MAX_ENTITIES];
        InitialEntityValues iev[MAX_ENTITIES];
        int index;
        int phase;
        
        Zeta::Handler *physics_handler;
    };

    bool add_entity(Scene *scene, E_::Entity *entity);
    bool remove_entity(Scene *scene, E_::Entity *entity);

    void setup(Scene *scene);
    void pause(Scene *scene);
    void reset(Scene *scene);
    void update(Scene *scene, float &time_step, RendererData *rd, Camera *camera, TexturesManager *tm, InputManager *im);
};
