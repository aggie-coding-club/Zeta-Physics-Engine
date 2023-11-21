#include "scene.h"
#include "renderer.h"
#include "ui.cpp"

namespace Scene{

    bool *add_entity(Scene *scene, Entity *entity){
        if(scene->phase != SCENE_PHASE_SETUP){
            // Assert()
            return false;
        }
        
        scene->entities[scene->index] = entity;
        scene->iev[scene->index] = entity;
        scene->index++;

        return true;
    }
    
    void remove_entity(Scene *scene, Entity *entity){
        if(scene->phase == SCENE_PHASE_SETUP){
            // Assert()
            return false;
        }
    }

    void setup(Scene *scene){
        scene->phase = SCENE_PHASE_PAUSED;
    }

    void toggle_pause(Scene *scene){
        if(scene->phase == SCENE_PHASE_PAUSED){
            scene->phase = SCENE_PHASE_PLAYING;
        }else if(scene->phase == SCENE_PHASE_PLAYING){
            scene->phase = SCENE_PHASE_PAUSED;
        } else{
            // Assert(!"invalid scene status");
        }  
    }

    void reset(Scene *scene){
        scene->phase = SCENE_PHASE_SETUP;
    }

    void update(Scene *scene, RendererData *rd, Camera *camera, TexturesManager *tm, InputManager *im){

        if(scene->phase == SCENE_PHASE_SETUP){
            scene->phase = SCENE_PHASE_PLAYING;
        }
        
        if(scene->phase == SCENE_PHASE_PLAYING){

        } else if(scene->phase == SCENE_PHASE_PAUSED){

        }else{
            // Assert(!"invalid scene status");
        }

        // draw entities
        render_entities(rd, camera, scene->entities, scene->index, tm, im);
    }
};