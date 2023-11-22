#include "scene.h"

namespace Scene{

    bool add_entity(Scene *scene, E_::Entity *entity){
        if(scene->phase != SCENE_PHASE_SETUP){
            Assert(!"not in setup phase!");
            return false;
        }
        
        scene->entities[scene->index] = entity;
        scene->iev[scene->index].entity_id = entity;

        if(entity->rb){
            Assert(scene->physics_handler);
            scene->physics_handler->addRigidBody(entity->rb);
        } else if(entity->sb){
            Assert(scene->physics_handler);
            scene->physics_handler->addStaticBody(entity->sb);
        }

        scene->index++;

        return true;
    }
    
    bool remove_entity(Scene *scene, E_::Entity *entity){
        if(scene->phase == SCENE_PHASE_SETUP){
            Assert(!"not in setup phase!");
            return false;
        }

        return true;
    }

    void setup(Scene *scene){
        scene->phase = SCENE_PHASE_SETUP;
        scene->physics_handler = new Zeta::Handler(ZMath::Vec3D(0, -5.8f, 0));
    }

    void toggle_pause(Scene *scene){
        if(scene->phase == SCENE_PHASE_PAUSED){
            scene->phase = SCENE_PHASE_PLAYING;
        }else if(scene->phase == SCENE_PHASE_PLAYING){
            scene->phase = SCENE_PHASE_PAUSED;
        } else{
            Assert(!"invalid scene status");
        }  
    }

    void play(Scene *scene){
        scene->phase = SCENE_PHASE_PLAYING;
    }

    void reset(Scene *scene){
        scene->phase = SCENE_PHASE_SETUP;
    }

    void update(Scene *scene, float time_step, RendererData *rd, Camera *camera, TexturesManager *tm, InputManager *im){
        if(scene->phase == SCENE_PHASE_PLAYING){

        } else if(scene->phase == SCENE_PHASE_PAUSED){

        }
        scene->physics_handler->update(time_step);
        // draw entities
        render_entities(rd, camera, scene->entities, scene->index, tm, im);
    }
};