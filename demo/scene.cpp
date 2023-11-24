#include "scene.h"

namespace Scene{

    bool add_entity(Scene *scene, E_::Entity *entity){
        if(scene->phase != SCENE_PHASE_SETUP){
            Assert(!"not in setup phase!");
            return false;
        }
        
        scene->entities[scene->entity_count] = entity;
        scene->iev[scene->entity_count].entity_id = entity;
        if(entity->rb){
            scene->iev[scene->entity_count].position = {entity->rb->pos.x, entity->rb->pos.y, entity->rb->pos.z};
        } else if(entity->sb){
            scene->iev[scene->entity_count].position = {entity->sb->pos.x, entity->sb->pos.y, entity->sb->pos.z};
        }

        if(entity->rb){
            Assert(scene->physics_handler);
            scene->physics_handler->addRigidBody(entity->rb);
        } else if(entity->sb){
            Assert(scene->physics_handler);
            scene->physics_handler->addStaticBody(entity->sb);
        }

        scene->entity_count++;

        return true;
    }
    
    bool remove_entity(Scene *scene, E_::Entity *entity){
        if(scene->phase != SCENE_PHASE_SETUP){
            Assert(!"not in setup phase!");
            return false;
        }

        return true;
    }
    
    Scene *new_scene(SceneManager *sm){
        Scene *result = 0;
        result = &sm->scenes[sm->index++];
        return result;
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

    void play(Scene *scene, float &time_step){
        scene->phase = SCENE_PHASE_PLAYING;
        // avoiding skipping because of time
        time_step = scene->time_paused;
    }
    
    void pause(Scene *scene, float time_step){
        scene->phase = SCENE_PHASE_PAUSED;
        scene->time_paused = time_step;
    }

    void reset(Scene *scene){
        scene->phase = SCENE_PHASE_SETUP;

        for(int i = 0; i < scene->entity_count; i++){
            E_::Entity *entity = scene->entities[i];
            InitialEntityValues iev = scene->iev[i];
            if(entity){
                if(entity->rb){
                    entity->rb->pos = {iev.position.X, iev.position.Y, iev.position.Z};
                } else if(entity->sb){
                    entity->sb->pos = {iev.position.X, iev.position.Y, iev.position.Z};
                }
            } else {
                Assert(!"Entity pointer not assigned in the middle of array");
            }
        }
    }

    void update(Scene *scene, float &time_step, RendererData *rd, Camera *camera, TexturesManager *tm, InputManager *im){
        if(scene->phase == SCENE_PHASE_PLAYING){
            int physics_updates = scene->physics_handler->update(time_step);

            for(int i = 0; i < scene->entity_count; i++){
                E_::Entity *entity = scene->entities[i];
                if(entity->physics_behavior){
                    entity->physics_behavior(entity, time_step, physics_updates);
                }
            }

        } else if(scene->phase == SCENE_PHASE_PAUSED){

        }
        // draw entities
        render_entities(rd, camera, scene->entities, scene->entity_count, tm, im);
    }
};