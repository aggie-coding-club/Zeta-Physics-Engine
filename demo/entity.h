#pragma once
#include "app.h"
#include <zeta/physicshandler.h>

#define MAX_ENTITIES 100
#define MAX_ENTITY_TEXTURES 4

namespace E_{
    struct Entity_{
        RawModel raw_model;
        unsigned int def_texture;
        unsigned int textures[MAX_ENTITY_TEXTURES] = {};
        unsigned int textureIndex;
        int isTransparent;
        
        HMM_Vec4 color = {};
        float scale = 0.0f;
        float rotation_x = 0.0f;
        float rotation_y = 0.0f;
        float rotation_z = 0.0f;

        Zeta::RigidBody3D *rb = 0;
        Zeta::StaticBody3D *sb = 0;
    };

    struct EntityManager{
        unsigned int index;
        Entity_ entities[MAX_ENTITIES];
    };
        
    static void ZetaVertsToEq(ZMath::Vec3D *zeta_verts, VertexData *vertex_data);

    Entity_ *CreateEntity(EntityManager *em, HMM_Vec3 position, float scale, 
            float rotation_x, float rotation_y, float rotation_z, Zeta::RigidBodyCollider colliderType, void *collider);

    Entity_ *CreateEntity(EntityManager*em, HMM_Vec3 position, float scale, 
            float rotation_x, float rotation_y, float rotation_z,  Zeta::StaticBodyCollider colliderType, void *collider);

    // call after `AddCollider()`
    void Init(Entity_ *entity);
                
    void Init(Entity_ *entity, RawModel model);

    // order matters
    void AddTexture(Entity_ *entity, unsigned int texture);

    static void AddCollider(Entity_ *entity, Zeta::RigidBodyCollider colliderType, void *collider);

    static void AddCollider(Entity_ *entity, Zeta::StaticBodyCollider colliderType, void *collider);

    static void IncreaseRotation(Entity_ *entity, float dx, float dy, float dz);
};