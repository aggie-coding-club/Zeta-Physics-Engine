#pragma once
#include "app.h"
#include <zeta/physicshandler.h>

#define MAX_ENTITIES 100

namespace E_{
    struct Entity_{
        RawModel raw_model;
        unsigned int def_texture;
        
        HMM_Vec4 color = {};
        float scale = 0.0f;
        float rotation_x = 0.0f;
        float rotation_y = 0.0f;
        float rotation_z = 0.0f;

        Primitives::RigidBody3D *rb = 0;
        Primitives::StaticBody3D *sb = 0;
    };

    struct EntityManager{
        unsigned int index;
        Entity_ entities[MAX_ENTITIES];
    };
        
    void ZetaVertsToEq(ZMath::Vec3D *zeta_verts, VertexData *vertex_data);

    Entity_ *CreateEntity(EntityManager *em, HMM_Vec3 position, float scale, 
            float rotation_x, float rotation_y, float rotation_z, Primitives::RigidBodyCollider colliderType, void *collider);

    Entity_ *CreateEntity(EntityManager*em, HMM_Vec3 position, float scale, 
        float rotation_x, float rotation_y, float rotation_z,  Primitives::StaticBodyCollider colliderType, void *collider);

    // call after `AddCollider()`
    void Init(Entity_ *entity);
                
    void Init(Entity_ *entity, RawModel model);

    void AddCollider(Entity_ *entity, Primitives::RigidBodyCollider colliderType, void *collider);

    void AddCollider(Entity_ *entity, Primitives::StaticBodyCollider colliderType, void *collider);

    void IncreaseRotation(Entity_ *entity, float dx, float dy, float dz);
};