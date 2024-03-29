#pragma once
#include "app.h"
#include <zeta/physicshandler.h>

#define MAX_ENTITIES 512
#define MAX_ENTITY_TEXTURES 4

namespace E_{
    struct Entity{
        bool initialized;
        bool highlighted;
        bool selected;
        unsigned int internal_identifier; // Starting with 1, Ending with 255
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
        void (*physics_behavior)(Entity*, float&, int);
    };

    struct EntityManager{
        unsigned int count;
        Entity entities[MAX_ENTITIES];
        Entity *entity_pointers[MAX_ENTITIES];
    };

    Entity *em_get_new_entity(EntityManager *em);

    void physics_verts_to_render_verts(ZMath::Vec3D *zeta_verts, VertexData *vertex_data);

    Entity *create_entity(EntityManager *em, HMM_Vec3 position, float scale, float mass,
            float rotation_x, float rotation_y, float rotation_z, Zeta::RigidBodyCollider colliderType, void *collider);

    Entity *create_entity(EntityManager*em, HMM_Vec3 position, float scale, 
            float rotation_x, float rotation_y, float rotation_z,  Zeta::StaticBodyCollider colliderType, void *collider);

    void init(Entity *entity, RawModel model);
    Entity *get_entity(Entity *entities[], unsigned int entity_count, unsigned int identifier);

    // order matters
    void add_texture(Entity *entity, unsigned int texture);

    static void add_collider(Entity *entity, Zeta::RigidBodyCollider colliderType, void *collider);

    static void add_collider(Entity *entity, Zeta::StaticBodyCollider colliderType, void *collider);

    static void increase_rotation(Entity *entity, float dx, float dy, float dz);
};